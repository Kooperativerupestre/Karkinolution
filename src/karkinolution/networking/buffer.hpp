#pragma once
#include <deque>
#include <karkinolution/binary/message_type_size.hpp>
#include <optional>
#include <vector>
inline constexpr int BUFFER_SIZE = 4096;


enum class BufferExistence : uint8_t {
	YES,
	NO,
	INCOMPLETE
};

class RequestBuffer {
	private:

		std::deque<std::byte> buffer_;

	public:

		RequestBuffer() {}

		RequestBuffer(std::deque<char> &buffer) {
			for (std::size_t i = buffer.size(); i > 0; --i) {
				buffer_.push_back(std::byte{static_cast<unsigned char>(buffer[i - 1])});
			}
		}

		RequestBuffer(std::deque<std::byte> buffer)
			: buffer_(buffer) {}

		const std::deque<std::byte> &view() const {
			return buffer_;
		}

		std::optional<std::byte> pop_front() {
			if (buffer_.size() == 0) {
				return std::nullopt;
			}
			std::byte byte = buffer_.front();
			buffer_.pop_front();
			return byte;
		}

		size_t clear() {
			size_t size = buffer_.size();
			buffer_.clear();
			return size;
		}

		bool add(std::byte byte) {
			if (buffer_.size() == BUFFER_SIZE) {
				return false;
			}
			buffer_.push_back(byte);
			return true;
		}

		bool add(char byte) {
			if (buffer_.size() > BUFFER_SIZE) {
				return false;
			}
			buffer_.push_back(std::byte(static_cast<unsigned char>(byte)));
			return true;
		}

		bool add(std::vector<char> bytes) {
			if (buffer_.size() + bytes.size() > BUFFER_SIZE) {
				return false;
			}
			for (char byte : bytes) {
				buffer_.push_back(std::byte(static_cast<unsigned char>(byte)));
			}
			return true;
		}

		template <size_t Size> bool add(std::array<char, Size> bytes) {
			if (buffer_.size() + Size > BUFFER_SIZE) {
				return false;
			}

			for (size_t i = 0; i < Size; i++) {
				buffer_.push_back(std::byte(static_cast<unsigned char>(bytes[i])));
			}
			return true;
		}
};

namespace FrameBufferAnalyzer {

	BufferExistence            has_size(const RequestBuffer &buffer);
	std::optional<std::size_t> get_size(const RequestBuffer &buffer);

	BufferExistence                             has_sub_type(const RequestBuffer &buffer);
	std::optional<BinarySubTypes::CodeSubTypes> get_sub_type(const RequestBuffer &buffer);

	BufferExistence            has_type(const RequestBuffer &buffer);
	std::optional<BinaryTypes> get_type(const RequestBuffer &buffer);

	BufferExistence                       has_payload(const RequestBuffer &buffer);
	std::optional<std::vector<std::byte>> get_payload(const RequestBuffer &buffer);


	BufferExistence                       has_frame(const RequestBuffer &buffer);
	std::optional<std::vector<std::byte>> get_frame(RequestBuffer &buffer);

} // namespace FrameBufferAnalyzer