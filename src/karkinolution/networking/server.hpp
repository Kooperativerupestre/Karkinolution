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

enum class ReceiveOverflowMode : uint8_t {
	VERIFY_ALL_BYTES,
	VERIFY_BYTE_PER_BYTE
};

inline constexpr ReceiveOverflowMode GLOBAL_RECEIVE_OVERFLOW_MODE =
	ReceiveOverflowMode::VERIFY_ALL_BYTES;

class ResponseBuffer {
	private:

		std::deque<std::vector<std::byte>> buffer_;

	public:

		ResponseBuffer() {}

		const std::deque<std::vector<std::byte>> &view() const {
			return buffer_;
		}

		void clear() {
			buffer_.clear();
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

		std::vector<std::byte> bytes;

		SendingState() {
			bytes.reserve(RESPONSE_BUFFER_MAX_SIZE);
		}
};

class Server {
	public:

		Server(IoContext &io_context, Port port, World &world);

		const World &world_view() const {
			return world;
		}

		~Server();

	private:

		void on_error(const std::string &error_message);
		void on_overflow_error(std::span<const std::byte> bytes) const;

		void on_error(const AsioErrorCode &error) {
			on_error(error.message());
		}

		void clear_sending_state();

		void clear_buffers();

		void clear();

		void close() {
			socket_.close();
		}

		void accept();

		void receive();

		void send_next();

		void send(const std::vector<std::byte> &);


		World   &world;
		Acceptor acceptor_;
		Socket   socket_;

		RequestBuffer request_buffer_;

		ResponseBuffer response_buffer_;

		SendingState sending_state;

		std::array<std::byte, REQUEST_BUFFER_MAX_SIZE> request_temporary_buffer;
};
