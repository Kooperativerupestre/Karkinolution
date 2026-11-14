#include "karkinolution/networking/client_side/client/client.hpp"

#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/client_side/client/alias.hpp"
#include "karkinolution/networking/client_side/client/pending_request.hpp"
#include "karkinolution/networking/client_side/client/protocol_session.hpp"
#include "karkinolution/networking/client_side/error.hpp"

#include <asio/bind_executor.hpp>
#include <asio/connect.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <format>
#include <iostream>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/frame/motor.hpp>
#include <karkinolution/networking/scope/motor.hpp>
#include <thread>

namespace Karkinolution::Networking::ClientSide::Client {
	Client::~Client() {
		stop_new_async_operations();
		wait_until_zero_async_operations();
	}

	void Client::on_error(const std::string &error_message) {
		std::cerr << "Error: " << error_message << "\n";
		disconnect();
	}

	void Client::disconnect() {
		asio::post(strand_, [this] {
			asio::error_code ec;
			socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
			socket_.close(ec);
			connected = false;
			protocol_session.fail_pending_requests();
		});
	}

	void Client::connect() {
		if (connected) {
			throw ClientError("Client is already connected");
		}

		Karkinolution::Networking::AsioResolver  resolver(io_context_);
		Karkinolution::Networking::AsioErrorCode error;

		const auto endpoints = resolver.resolve(config.host, std::to_string(config.port), error);

		if (error) {
			throw ClientError(std::format("Failed to resolve {}:{}: {}", config.host, config.port, error.message()));
		}

		error.clear();

		asio::connect(socket_, endpoints, error);

		if (error) {
			throw ClientError(std::format("Failed to connect to {}:{}: {}", config.host, config.port, error.message()));
		}
		connected = true;
		asio::post(strand_, [this] {
			receive();
		});
	}

	RequestId Client::send_without_waiting(const RequestData &data) {
		Validator::validate_is_connected(*this);

		const auto request_id = next_request_id.fetch_add(1);
		auto       raw_frame  = Karkinolution::Networking::Scope::Motor::make_world_frame(
            Karkinolution::Binary::Frames::Motor::make_request_raw_bytes(data.sub_type,
                                                                         request_id,
                                                                         data.resource,
                                                                         data.payload,
                                                                         data.extra));

		protocol_session.register_request(request_id);
		asio::post(strand_, [this, frame = std::move(raw_frame)]() mutable {
			protocol_session.enqueue_request(frame);
			send();
		});

		return request_id;
	}

	Protocol::SystemResponseResult Client::request(const SystemRequestData &data) {
		Validator::validate_is_connected(*this);
		const auto request_id = next_request_id.fetch_add(1);
		const auto frame      = Karkinolution::Networking::Scope::Motor::make_system_frame(
            Karkinolution::Networking::Frames::Motor::make_raw_system_call(data.sub_type, request_id, data.payload));
		protocol_session.register_system_request(request_id);
		asio::post(strand_, [this, frame] {
			protocol_session.enqueue_request(frame);
			send();
		});
		const auto output = protocol_session.await_system_response(request_id);
		if (!output) {
			if (output.error() == Protocol::RequestFailure::DISCONNECTION) {
				throw ClientError("Tried to request with the client disconnected");
			}
			return std::unexpected(Protocol::RequestFailure::CANCEL);
		}
		return output;
	}

	void Client::send() {
		if (!connected || sending_state.is_sending || stopped_new_async_operations) {
			return;
		}

		auto bytes = protocol_session.pop_request();

		if (!bytes) {
			return;
		}

		current_async_operations_running.increment();
		sending_state.is_sending = true;
		sending_state.bytes      = bytes.value();
		try {
			asio::async_write(
				socket_,
				asio::buffer(sending_state.bytes.data(), sending_state.bytes.size()),

				asio::bind_executor(strand_,
									[this](const Karkinolution::Networking::AsioErrorCode &error, std::size_t) {
										if (error) {
											if (error == asio::error::operation_aborted) {
												current_async_operations_running.decrement();
												return;
											}
											on_error(error.message());
											current_async_operations_running.decrement();
											return;
										}
										clear_sending_state();

										std::cout << "Request sent 🦀\n";
										send();
										current_async_operations_running.decrement();
									}));
		} catch (...) {
			current_async_operations_running.decrement();
			throw;
		}
	}

	void Client::receive() {
		if (stopped_new_async_operations) {
			return;
		}
		current_async_operations_running.increment();
		try {
			socket_.async_read_some(
				asio::buffer(temporary_responses),
				asio::bind_executor(
					strand_,
					[this](const Karkinolution::Networking::AsioErrorCode &error, std::size_t bytes_received) {
						if (error) {
							if (error == asio::error::eof) {
								on_closed();
								current_async_operations_running.decrement();
								return;
							} else if (error == asio::error::operation_aborted) {
								current_async_operations_running.decrement();
								return;
							}

							on_error(error.message());
							current_async_operations_running.decrement();
							return;
						}

						protocol_session.receive(
							std::span<const std::byte>{temporary_responses.data(), bytes_received});
						receive();
						current_async_operations_running.decrement();
					}));
		} catch (...) {
			current_async_operations_running.decrement();
			throw;
		}
	}

	ResponseResult Client::pop_response(RequestId id) {
		const auto output = protocol_session.pop_response(id);

		if (!output) {
			if (output.error() == Protocol::RequestFailure::DISCONNECTION) {
				throw ClientError("Tried to pop a response with the client disconnected");
			} else if (output.error() == Protocol::RequestFailure::CANCEL) {
				return std::unexpected(Protocol::CancelledError{});
			}
		}
		return output.value();
	}

	ResponseResult Client::request(const RequestData &data) {
		const auto id     = send_without_waiting(data);
		const auto output = protocol_session.await_response(id);

		if (!output) {
			if (output.error() == Protocol::RequestFailure::DISCONNECTION) {
				throw ClientError("Tried to request with the client disconnected");
			}

			return std::unexpected(Protocol::CancelledError{});
		}

		return output.value();
	}

	void Client::stop_new_async_operations() {
		auto promise = std::make_shared<std::promise<void>>();
		auto future  = promise->get_future();

		asio::post(strand_, [this, promise] {
			stopped_new_async_operations.store(true);
			asio::error_code ec;
			socket_.cancel(ec);
			promise->set_value();
		});
		future.get();
	}
} // namespace Karkinolution::Networking::ClientSide::Client
