#pragma once
#include "karkinolution/binary/frames/parser.hpp"

#include <deque>
#include <karkinolution/binary/protocol/layouts.hpp>
#include <optional>
#include <vector>
inline constexpr int MAX_RAW_FRAME_BUFFER_SIZE = 4096;


using BufferExistence = Frames::Parser::FrameFieldExistence;
using BinaryTypes     = Karkinolution::Binary::Protocol::Types::BinaryTypes;
using CodeSubTypes    = Karkinolution::Binary::Protocol::Types::BinarySubTypes::CodeSubTypes;

/*
Why is this used by Client and ProtocolSession (Server side)
 For example:

 A_storage <- A <- Receive request <- B
 B_storage <- B <- Receive response <- A

 The underlying logic is the same.

 */

class RawFrameBuffer {
	private:

		std::deque<std::byte> buffer_;

	public:

		RawFrameBuffer() {}

		RawFrameBuffer(std::deque<char> &buffer) {
			for (std::size_t i = 0; i < buffer.size(); i++) {
				buffer_.push_back(std::byte{static_cast<unsigned char>(buffer[i])});
			}
		}

		RawFrameBuffer(std::deque<std::byte> buffer)
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
			if (buffer_.size() >= MAX_RAW_FRAME_BUFFER_SIZE) {
				return false;
			}
			buffer_.push_back(byte);
			return true;
		}

		bool add(char byte) {
			if (buffer_.size() >= MAX_RAW_FRAME_BUFFER_SIZE) {
				return false;
			}
			buffer_.push_back(std::byte(static_cast<unsigned char>(byte)));
			return true;
		}

		bool add(std::span<const std::byte> bytes) {
			if (buffer_.size() + bytes.size() > MAX_RAW_FRAME_BUFFER_SIZE) {
				return false;
			}
			for (auto byte : bytes) {
				buffer_.push_back(byte);
			}
			return true;
		}

		bool add(std::vector<char> bytes) {
			if (buffer_.size() + bytes.size() > MAX_RAW_FRAME_BUFFER_SIZE) {
				return false;
			}
			for (char byte : bytes) {
				buffer_.push_back(std::byte(static_cast<unsigned char>(byte)));
			}
			return true;
		}

		template <size_t Size> bool add(std::array<char, Size> bytes) {
			if (buffer_.size() + Size > MAX_RAW_FRAME_BUFFER_SIZE) {
				return false;
			}

			for (size_t i = 0; i < Size; i++) {
				buffer_.push_back(std::byte(static_cast<unsigned char>(bytes[i])));
			}
			return true;
		}
};

inline constexpr std::size_t MAX_PAYLOAD_BYTES = std::min(
	static_cast<std::size_t>(UINT32_MAX)
		- Karkinolution::Binary::Protocol::Layout::TOTAL_HEADER_SIZE,
	MAX_RAW_FRAME_BUFFER_SIZE - Karkinolution::Binary::Protocol::Layout::TOTAL_HEADER_SIZE);

namespace FrameBufferAnalyzer {

	BufferExistence has_size(const RawFrameBuffer &buffer);

	std::optional<std::size_t> get_size(const RawFrameBuffer &buffer);

	BufferExistence has_sub_type(const RawFrameBuffer &buffer);

	std::optional<CodeSubTypes> get_sub_type(const RawFrameBuffer &buffer);

	BufferExistence has_type(const RawFrameBuffer &buffer);

	std::optional<BinaryTypes> get_type(const RawFrameBuffer &buffer);

	BufferExistence has_payload(const RawFrameBuffer &buffer);

	std::optional<std::vector<std::byte>> get_payload(const RawFrameBuffer &buffer);


	BufferExistence has_frame(const RawFrameBuffer &buffer);

	std::optional<std::vector<std::byte>> get_frame(RawFrameBuffer &buffer);

} // namespace FrameBufferAnalyzer
