#include "karkinolution/global_aliasses.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/core/id.hpp"

#include <asio/buffer.hpp>
#include <asio/write.hpp>
#include <iostream>
#include <karkinolution/networking/connection/connection.hpp>
#include <karkinolution/networking/frame/motor.hpp>
#include <karkinolution/networking/scope/motor.hpp>
#include <variant>

namespace Karkinolution::Networking::Connection {

	void Connection::on_error(const std::string &error_message) {
		std::cerr << "Error: " << error_message << "\n";

		on_closed(*this);

		close();
	}

	void Connection::clear_sending_state() {
		sending_state.bytes.clear();
		sending_state.is_sending = false;
	}

	void Connection::start() {
		receive();
	}

	Connection::Connection(Socket socket, Simulation::Simulation &simulation)
		: id(next_id.fetch_add(1))
		, simulation_adapter(simulation, id)
		, protocol_session(simulation_adapter)
		, socket_(std::move(socket)) {}

	void Connection::receive() {
		socket_.async_read_some(asio::buffer(temporary_requests),

								[this](const AsioErrorCode &error, std::size_t bytes_received) {
									if (error) {

										if (error == asio::error::eof) {
											on_closed(*this);
											close();
											return;
										}

										on_error(error.message());
										return;
									}

									try {
										protocol_session.receive(std::span(temporary_requests).first(bytes_received));
									} catch (const std::exception &exception) {
										on_error(exception.what());
										return;
									}

									send_next();


									receive();
								});
	}

	void Connection::send_next() {

		if (sending_state.is_sending) {
			return;
		}

		if (!protocol_session.has_response()) {
			return;
		}

		auto response = protocol_session.pop_response();

		sending_state.is_sending = true;


		sending_state.bytes = std::visit(
			[](const auto &response) {
				using Response = std::decay_t<decltype(response)>;
				if constexpr (std::is_same_v<Response, Frames::Models::SystemOutput::SystemOutput>) {
					return Scope::Motor::make_system_frame(
						Frames::Motor::make_raw_system_output(response.get_sub_type(),
															  response.get_call_id(),
															  response.get_payload()));
				} else {
					return Scope::Motor::make_world_frame(
						Binary::Frames::Motor::make_raw_response(response.get_cardinality(),
																 response.get_sub_type(),
																 response.get_request_id(),
																 response.get_payload()));
				}
			},
			response);

		asio::async_write(socket_,
						  asio::buffer(sending_state.bytes.data(), sending_state.bytes.size()),

						  [this](const AsioErrorCode &error, std::size_t) {
							  if (error) {
								  on_error(error.message());
								  return;
							  }

							  clear_sending_state();

							  std::cout << "Response sent 🦀\n";

							  send_next();
						  });
	}

	void Connection::close() {
		socket_.close();
	}

} // namespace Karkinolution::Networking::Connection
