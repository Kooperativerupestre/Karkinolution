#include <asio/buffer.hpp>
#include <asio/write.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/networking/connection.hpp>

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

Connection::Connection(Socket socket, World &world)
	: world(world)
	, socket_(std::move(socket))
	, protocol_session(world) {}

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

								protocol_session.receive(
									std::span(temporary_requests).first(bytes_received));

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


	sending_state.bytes =
		Karkinolution::Binary::Frames::Motor::make_response_raw_bytes(response.type,
																	  response.sub_type,
																	  response.request_id,
																	  response.payload);

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