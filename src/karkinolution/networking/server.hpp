#pragma once

#include "karkinolution/binary/frames/processor.hpp"
#include "karkinolution/core/error.hpp"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <karkinolution/networking/buffer.hpp>
#include <karkinolution/world/world.hpp>
using Acceptor      = asio::ip::tcp::acceptor;
using Socket        = asio::ip::tcp::socket;
using IoContext     = asio::io_context;
using Port          = unsigned short;
using AsioErrorCode = asio::error_code;

inline constexpr size_t RESPONSE_BUFFER_MAX_SIZE = 20;

class ResponseBuffer {
	private:

		std::deque<std::vector<std::byte>> buffer_;

	public:

		ResponseBuffer() {}

		const std::deque<std::vector<std::byte>> &view() const {
			return buffer_;
		}

		bool add(const std::vector<std::byte> &bytes) {
			if (buffer_.size() >= RESPONSE_BUFFER_MAX_SIZE) {
				return false;
			}
			buffer_.push_back(bytes);
			return true;
		}

		template <size_t Size> bool add(const std::array<std::byte, Size> &bytes) {
			if (buffer_.size() >= RESPONSE_BUFFER_MAX_SIZE) {
				return false;
			}
			buffer_.push_back(bytes);
			return true;
		}

		bool add(const std::vector<char> &bytes) {
			if (buffer_.size() >= RESPONSE_BUFFER_MAX_SIZE) {
				return false;
			}
			std::vector<std::byte> casted;
			casted.reserve(bytes.size());

			for (char byte : bytes) {
				casted.push_back(std::byte(static_cast<unsigned char>(byte)));
			}
			buffer_.push_back(std::move(casted));
			return true;
		}

		template <size_t Size> bool add(const std::array<char, Size> &bytes) {
			if (buffer_.size() >= RESPONSE_BUFFER_MAX_SIZE) {
				return false;
			}

			std::vector<std::byte> casted;
			casted.reserve(bytes.size());

			for (char byte : bytes) {
				casted.push_back(std::byte(static_cast<unsigned char>(byte)));
			}
			buffer_.push_back(std::move(casted));
			return true;
		}

		std::vector<std::byte> pop_front() {
			if (buffer_.empty()) {
				throw BufferError("The response buffer does not have enough bytes.");
			}

			auto value = std::move(buffer_.front());
			buffer_.pop_front();

			return value;
		}
};

struct SendingState {
	public:

		bool is_sending = false;

		std::unique_ptr<std::vector<std::byte>> bytes = nullptr;
};

class Server {
	public:

		Server(IoContext &io_context, Port port, World &world);

		const World &world_view() const {
			return world;
		}

		~Server();

	private:


		void accept();
		void receive();
		void send_next();
		void send(const std::vector<std::byte> &);

		void move_temporary_buffer_to_request_buffer();

		World   &world;
		Acceptor acceptor_;
		Socket   socket_;

		RequestBuffer request_buffer_;

		ResponseBuffer response_buffer_;

		SendingState sending_state;

		FrameProcessor frame_processor;

		std::array<char, BUFFER_SIZE> temporary_buffer;
};