

#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/buffer.hpp"

#include <asio/write.hpp>
#include <iostream>
#include <karkinolution/networking/server.hpp>

Server::Server(IoContext &io_context, Port port, World &world)
	: acceptor_(io_context)
	, socket_(io_context)
	, world(world) {
	acceptor_.open(asio::ip::tcp::v4());
	acceptor_.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));

	acceptor_.listen();
	accept();
}

void Server::send_next() {
	if (response_buffer_.view().empty()) {
		return;
	}

	if (sending_state.is_sending) {
		return;
	}
	sending_state.is_sending = true;
	sending_state.bytes = std::make_unique<std::vector<std::byte>>(response_buffer_.pop_front());

	asio::async_write(socket_,
					  asio::buffer(sending_state.bytes->data(), sending_state.bytes->size()),
					  [this](const AsioErrorCode &error, std::size_t bytes_sent) {
						  if (error) {
							  std::cerr << "Send error: " << error.message() << "\n";
							  sending_state.is_sending = false;
							  sending_state.bytes.reset();
							  return;
						  }

						  sending_state.bytes.reset();
						  sending_state.is_sending = false;

						  std::cout << "Response sent 🦀\n";

						  if (!response_buffer_.view().empty()) {
							  send_next();
						  }
					  });
}

void Server::send(const std::vector<std::byte> &bytes) {
	if (!response_buffer_.add(bytes)) {
		return;
	}

	if (sending_state.is_sending) {
		return;
	}
	send_next();
}

void Server::receive() {
	socket_.async_read_some(asio::buffer(temporary_buffer),
							[this](const AsioErrorCode &error, std::size_t bytes_received) {
								if (error) {
									std::cerr << "Receive error: " << error.message() << "\n";
									return;
								}

								for (size_t i = 0; i < bytes_received; i++) {
									request_buffer_.add(temporary_buffer[i]);
								}


								while (FrameBufferAnalyzer::has_frame(request_buffer_)
									   == BufferExistence::YES) {


									auto frame = FrameBufferAnalyzer::get_frame(request_buffer_);


									auto parsed_frame = FrameParser::parse_frame(frame.value());


									frame_processor.add(parsed_frame);


									auto response = frame_processor.process(world);


									send(response);
								}

								receive();
							});
}

void Server::accept() {
	acceptor_.async_accept(socket_, [this](const AsioErrorCode &error) {
		if (!error) {
			std::cout << "I'm fucking alive🦀" << std::endl;
			receive();
		}
	});
}

Server::~Server() {
	std::cout << "Server was killed 🦀\n";
}