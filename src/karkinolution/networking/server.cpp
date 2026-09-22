#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/buffer.hpp"

#include <asio/write.hpp>
#include <iomanip>
#include <iostream>
#include <karkinolution/networking/server.hpp>
#include <span>

void Server::on_error(const std::string &error_message) {
	std::cerr << "Error: " << error_message << "\n";
	clear();
	close();
}

void Server::on_overflow_error(std::span<const std::byte> bytes) const {
	std::cerr << "Overflow error: " << bytes.size() << " (discarded bytes) + "
			  << request_buffer_.view().size() << " (request buffer size) > "
			  << REQUEST_BUFFER_MAX_SIZE << "\n";

	for (const auto byte : bytes) {
		std::cerr << std::hex << std::setw(2) << std::setfill('0')
				  << std::to_integer<unsigned int>(byte) << ' ';
	}

	std::cerr << std::dec << '\n';
}

void Server::clear_sending_state() {
	sending_state.bytes.clear();
	sending_state.is_sending = false;
}

void Server::clear_buffers() {
	response_buffer_.clear();
	request_buffer_.clear();
}

void Server::clear() {
	clear_sending_state();
	clear_buffers();
}

Server::Server(IoContext &io_context, Port port, World &world)
	: acceptor_(io_context)
	, socket_(io_context)
	, world(world) {
	if constexpr (GLOBAL_RECEIVE_OVERFLOW_MODE == ReceiveOverflowMode::VERIFY_ALL_BYTES) {
		std::cout << "The receive overflow mode is: verify all bytes before receive\n";
	} else if constexpr (GLOBAL_RECEIVE_OVERFLOW_MODE
						 == ReceiveOverflowMode::VERIFY_BYTE_PER_BYTE) {
		std::cout << "The receive overflow mode is: verify byte per byte while receiving\n";
	}

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
	sending_state.bytes      = response_buffer_.pop_front();

	asio::async_write(socket_,
					  asio::buffer(sending_state.bytes.data(), sending_state.bytes.size()),
					  [this](const AsioErrorCode &error, std::size_t) {
						  if (error) {
							  on_error(error);
							  return;
						  }

						  clear_sending_state();

						  std::cout << "Response sent 🦀\n";

						  if (!response_buffer_.view().empty()) {
							  send_next();
						  }
					  });
}

void Server::send(const std::vector<std::byte> &bytes) {
	if (!response_buffer_.add(bytes)) {
		on_overflow_error(bytes);
		return;
	}

	if (sending_state.is_sending) {
		return;
	}

	send_next();
}

void Server::receive() {
	socket_.async_read_some(
		asio::buffer(request_temporary_buffer),
		[this](const AsioErrorCode &error, std::size_t bytes_received) {
			if (error) {
				on_error(error);
				return;
			}


			if constexpr (GLOBAL_RECEIVE_OVERFLOW_MODE
						  == ReceiveOverflowMode::VERIFY_BYTE_PER_BYTE) {
				for (std::size_t i = 0; i < bytes_received; ++i) {
					if (!request_buffer_.add(request_temporary_buffer[i])) {
						on_overflow_error(
							std::span(request_temporary_buffer).subspan(i, bytes_received - i));
						break;
					}
				}
			} else if constexpr (GLOBAL_RECEIVE_OVERFLOW_MODE
								 == ReceiveOverflowMode::VERIFY_ALL_BYTES) {
				if (bytes_received + request_buffer_.view().size() > REQUEST_BUFFER_MAX_SIZE) {
					on_overflow_error(std::span(request_temporary_buffer).first(bytes_received));
				} else {
					for (std::size_t i = 0; i < bytes_received; ++i) {
						request_buffer_.add(request_temporary_buffer[i]);
					}
				}
			}

			try {
				while (FrameBufferAnalyzer::has_frame(request_buffer_) == BufferExistence::YES) {
					auto frame = FrameBufferAnalyzer::get_frame(request_buffer_);

					auto parsed_frame = FrameParser::parse_frame(frame.value());

					auto response = FrameProcessor::process(parsed_frame, world);

					send(response);
				}
			} catch (const std::exception &e) {
				on_error(std::string("Malformed frame: ") + e.what());
				return;
			}
			receive();
		});
}

void Server::accept() {
	acceptor_.async_accept(socket_, [this](const AsioErrorCode &error) {
		if (!error) {
			std::cout << "I'm fucking alive🦀\n";
			receive();
		} else {
			on_error(error);
		}
	});
}

Server::~Server() {
	std::cout << "Server was killed 🦀\n";
}