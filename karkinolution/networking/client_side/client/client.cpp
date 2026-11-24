#include "karkinolution/networking/client_side/client/client.hpp"

#include "karkinolution/networking/client_side/client/alias.hpp"
#include "karkinolution/networking/client_side/client/pending_request.hpp"
#include "karkinolution/networking/client_side/client/protocol_session.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/frame/system/contract.hpp"

#include <asio/bind_executor.hpp>
#include <asio/connect.hpp>
#include <asio/post.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <exception>
#include <format>
#include <iostream>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/frame/system/motor.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <karkinolution/networking/scope/motor.hpp>
#include <shared/functional/scope_guard.hpp>
#include <thread>

namespace Karkinolution::Networking::ClientSide {

	void SendingState::reset_() {
		raw_frame_.reset();
		is_sending_ = false;
		scope_.reset();
		has_finished_.reset();
	}

	SendingState::operator bool() const noexcept {
		return is_sending_;
	}

	bool SendingState::reset_if_finished() {
		if (!has_finished_.value_or(false)) {
			return false;
		}
		reset_();
		return true;
	}

	asio::const_buffer SendingState::take_buffer() const {
		if (has_finished_.value_or(false) && raw_frame_) {
			return asio::buffer(*raw_frame_);
		}
		return asio::const_buffer{};
	}

	bool SendingState::prepare_sending_state(Scope::Scope scope) {
		if (is_prepared()) {
			return false;
		}
		scope_ = scope;

		is_sending_   = true;
		has_finished_ = false;
		raw_frame_.emplace();
		return true;
	}

	bool SendingState::enqueue_raw_bytes(std::span<const std::byte> new_bytes) {
		if (has_finished_.value_or(false) || !is_sending()) {
			return false;
		}
		assert(raw_frame_);

		for (size_t i = 0; i < new_bytes.size(); i++) {
			raw_frame_->push_back(new_bytes[i]);
		}
		return true;
	}

	bool SendingState::mark_as_finished() {
		if (!is_sending_) {
			return false;
		}
		has_finished_ = true;
		return true;
	}

	namespace Client {

		void AsyncOperationsRunning::increment() {
			std::lock_guard lock{mutex};
			count++;
		}

		void AsyncOperationsRunning::decrement() {
			std::lock_guard lock{mutex};
			count--;
			if (count == 0) {
				cv.notify_all();
			}
		}

		int AsyncOperationsRunning::load() {
			std::lock_guard lock{mutex};
			return count;
		}

		void AsyncOperationsRunning::wait_until_zero() {
			std::unique_lock lock{mutex};
			cv.wait(lock, [this] {
				return count == 0;
			});
		}

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
				session.fail_pending_requests();
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
				throw ClientError(
					std::format("Failed to resolve {}:{}: {}", config.host, config.port, error.message()));
			}

			error.clear();

			asio::connect(socket_, endpoints, error);

			if (error) {
				throw ClientError(
					std::format("Failed to connect to {}:{}: {}", config.host, config.port, error.message()));
			}
			connected = true;
			asio::post(strand_, [this] {
				receive();
			});
		}

		RequestId Client::send_without_waiting_world_scope(const WorldRequestData &data) {
			Validator::validate_is_connected(*this);

			const auto request_id = next_request_id.fetch_add(1);
			auto       raw_frame  = Karkinolution::Networking::Scope::Motor::make_world_frame(
                Karkinolution::Networking::Frames::World::Motor::make_request_raw_bytes(data.sub_type,
                                                                                        request_id,
                                                                                        data.resource,
                                                                                        data.payload,
                                                                                        data.extra));

			session.register_world_request(request_id);
			asio::post(strand_, [this, frame = std::move(raw_frame)]() mutable {
				session.enqueue_world_request(frame);
				send();
			});

			return request_id;
		}

		WorldResponseResult Client::request_world_scope(const WorldRequestData &data) {
			Validator::validate_is_connected(*this);

			const auto request_id = send_without_waiting_world_scope(data);
			const auto output     = session.await_world_response(request_id);

			if (!output) {
				if (output.error() == Protocol::RequestFailure::DISCONNECTION) {
					throw ClientError("Tried to request with the client disconnected");
				}
				return std::unexpected(Protocol::RequestFailure::CANCEL);
			}
			return output;
		}

		WorldResponseResult Client::pop_response_world_scope(Frames::World::RequestId id) {
			const auto output = session.pop_world_response(id);

			if (!output) {
				if (output.error() == Protocol::RequestFailure::DISCONNECTION) {
					throw ClientError("Tried to pop a response with the client disconnected");
				}
				return std::unexpected(Protocol::RequestFailure::CANCEL);
			}
			return output;
		}

		Frames::System::SystemCall::Id Client::send_without_waiting_system_scope(const SystemRequestData &data) {
			Validator::validate_is_connected(*this);

			const auto request_id = next_request_id.fetch_add(1);
			const auto raw_Frame  = Scope::Motor::make_system_frame(
                Frames::System::Motor::make_raw_system_call(data.sub_type, request_id, data.payload, data.extra));


			session.register_system_request(request_id);

			asio::post(strand_, [this, frame = std::move(raw_Frame)]() mutable {
				session.enqueue_system_request(frame);
				send();
			});
			return request_id;
		}

		SystemResponseResult Client::pop_response_system_scope(Frames::System::SystemCall::Id id) {
			const auto output = session.pop_system_response(id);

			if (!output) {
				if (output.error() == Protocol::RequestFailure::DISCONNECTION) {
					throw ClientError("Tried to pop a response with the client disconnected");
				}
				return std::unexpected(Protocol::RequestFailure::CANCEL);
			}
			return output;
		}

		SystemResponseResult Client::request_system_scope(const SystemRequestData &data) {
			Validator::validate_is_connected(*this);
			const auto request_id = send_without_waiting_system_scope(data);
			const auto output     = session.await_system_response(request_id);
			if (!output) {
				if (output.error() == Protocol::RequestFailure::DISCONNECTION) {
					throw ClientError("Tried to request with the client disconnected");
				}
				return std::unexpected(Protocol::RequestFailure::CANCEL);
			}
			return output;
		}

		void Client::send() {
			if (!connected || sending_state || stopped_new_async_operations) {
				return;
			}

			auto bytes = session.pop_request_front();

			if (!bytes) {
				return;
			}


			sending_state.prepare_sending_state(bytes.see_scope().value());
			sending_state.enqueue_raw_bytes(bytes.see_frame().value());


			sending_state.mark_as_finished();
			current_async_operations_running.increment();
			try {
				asio::async_write(
					socket_,
					sending_state.take_buffer(),

					asio::bind_executor(strand_,
										[this](const Karkinolution::Networking::AsioErrorCode &error, std::size_t) {
											KShared::Functional::ScopeGuard scope_guard{[this]() noexcept {
												current_async_operations_running.decrement();
											}};
											if (error) {
												if (error == asio::error::operation_aborted) {
													return;
												}
												on_error(error.message());
												return;
											}
											const auto has_reseted = sending_state.reset_if_finished();
											assert(has_reseted);
											send();
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
							KShared::Functional::ScopeGuard scope_guard{[this]() noexcept {
								current_async_operations_running.decrement();
							}};

							if (error) {
								if (error == asio::error::eof) {
									on_closed();
									return;
								} else if (error == asio::error::operation_aborted) {
									return;
								}

								on_error(error.message());
								return;
							}
							try {
								session.receive(std::span<const std::byte>{temporary_responses.data(), bytes_received});
							} catch (const std::exception &e) {
								on_error(e.what());
								return;
							}

							receive();
						}));
			} catch (...) {
				current_async_operations_running.decrement();
				throw;
			}
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

		RuntimeClient::RuntimeClient(Config config, std::string name)
			: io()
			, guard(io.get_executor())
			, config(std::move(config))
			, name(std::move(name))
			, client(std::in_place, io, this->config, this->name)
			, runner([this] {
				io.run();
			}) {}

		RuntimeClient::~RuntimeClient() noexcept {
			disconnect();
			guard.reset();
			if (runner.joinable()) {
				runner.join();
			}
		}

		bool RuntimeClient::is_connected() const noexcept {
			return client.has_value() && client->is_connected();
		}

		bool RuntimeClient::connect() noexcept {
			try {
				if (is_connected()) {
					return true;
				}
				client.emplace(io, config, name);
				client->connect();
				return client->is_connected();
			} catch (...) {
				client.reset();
				return false;
			}
		}

		bool RuntimeClient::disconnect() noexcept {
			try {
				if (client) {
					if (client->is_connected()) {
						client->disconnect();
					}
					client->stop_new_async_operations();
					client->wait_until_zero_async_operations();
					client.reset();
				}
				return true;
			} catch (...) {
				return false;
			}
		}

		SystemResponseResult RuntimeClient::pop_response_system_scope(Frames::System::SystemCall::Id id) {
			Validator::validate_is_connected(*this);

			return client->pop_response_system_scope(id);
		}

		SystemResponseResult RuntimeClient::request_system_scope(const SystemRequestData &data) {
			Validator::validate_is_connected(*this);

			return client->request_system_scope(data);
		}

		Frames::System::SystemCall::Id RuntimeClient::send_without_waiting_system_scope(const SystemRequestData &data) {
			Validator::validate_is_connected(*this);

			return client->send_without_waiting_system_scope(data);
		}

		WorldResponseResult RuntimeClient::pop_response_world_scope(Frames::World::RequestId id) {
			Validator::validate_is_connected(*this);
			return client->pop_response_world_scope(id);
		}

		WorldResponseResult RuntimeClient::request_world_scope(const WorldRequestData &data) {
			Validator::validate_is_connected(*this);
			return client->request_world_scope(data);
		}

		Frames::World::RequestId RuntimeClient::send_without_waiting_world_scope(const WorldRequestData &data) {
			Validator::validate_is_connected(*this);
			return client->send_without_waiting_world_scope(data);
		}

	} // namespace Client
} // namespace Karkinolution::Networking::ClientSide