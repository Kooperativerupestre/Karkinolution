#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/protocol_session.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/networking/sdk/src/client/pending_request.hpp"
#include "karkinolution/networking/sdk/src/error.hpp"

#include <asio/bind_executor.hpp>
#include <asio/connect.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/sdk/src/client/client.hpp>
#include <thread>
using KarkinolutionSDK::Client::Client;

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
		throw ConnectionError("Client is already connected");
	}

	AsioResolver resolver(io_context_);

	AsioErrorCode error;

	const auto endpoints = resolver.resolve(config.host, std::to_string(config.port), error);

	if (error) {
		throw ConnectionError(
			std::format("Failed to resolve {}:{}: {}", config.host, config.port, error.message()));
	}

	error.clear();

	asio::connect(socket_, endpoints, error);

	if (error) {
		throw ConnectionError(std::format("Failed to connect to {}:{}: {}",
										  config.host,
										  config.port,
										  error.message()));
	}
	connected = true;
	asio::post(strand_, [this] {
		receive();
	});
}

RequestId Client::send_without_waiting(const RequestData &data) {
	// Throwing here is intended: this runs on the user's thread, so the user
	// can catch and handle a disconnected client.
	KarkinolutionSDK::Client::Validator::validate_is_connected(*this);

	const auto request_id = next_request_id.fetch_add(1);
	auto raw_frame = Karkinolution::Binary::Frames::Motor::make_request_raw_bytes(data.sub_type,
																				  request_id,
																				  data.resource,
																				  data.payload,
																				  data.extra);


	protocol_session.register_request(request_id);
	asio::post(strand_, [this, frame = std::move(raw_frame)]() mutable {
		protocol_session.enqueue_request(frame);
		send();
	});

	return request_id;
}

void Client::send() {
	// The connection can drop between the check in the public API and this
	// call (TOCTOU). Disconnection is a normal condition, so don't throw:
	// an exception escaping a handler would terminate the io_context thread.
	// Also skip if a write is already in progress.
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

			asio::bind_executor(strand_, [this](const AsioErrorCode &error, std::size_t) {
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
			asio::bind_executor(strand_,
								[this](const AsioErrorCode &error, std::size_t bytes_received) {
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
										std::span<const std::byte>{temporary_responses.data(),
																   bytes_received});
									current_async_operations_running.decrement();
								}));
	} catch (...) {
		current_async_operations_running.decrement();
		throw;
	}
}

KarkinolutionSDK::Client::ResponseResult Client::pop_response(RequestId id) {
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

KarkinolutionSDK::Client::ResponseResult Client::request(const RequestData &data) {
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
		// The socket may already be closed (e.g. server dropped the connection
		// before this is called). Use the error_code overload so we don't throw
		// on a bad file descriptor.
		asio::error_code ec;
		socket_.cancel(ec);
		promise->set_value();
	});
	future.get();
}