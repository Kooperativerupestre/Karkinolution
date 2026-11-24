#pragma once
#include "karkinolution/networking/frame/world/parser/internal.hpp"
#include "karkinolution/networking/frame/world/parser/parser.hpp"
#include "karkinolution/networking/frame/world/protocol/contract.hpp"

#include <cstdint>
#include <deque>
#include <karkinolution/networking/frame/world/protocol/layouts.hpp>
#include <karkinolution/networking/scope/layout.hpp>
#include <optional>
#include <vector>

namespace Karkinolution::Networking {

	inline constexpr int MAX_RAW_FRAME_BUFFER_SIZE = 64 * 1024;


	using BufferExistence = Karkinolution::Networking::Frames::World::Parser::Internal::FrameExistence;

	/*
	Why is this used by Client and ProtocolSession (Server side)
	 For example:

	 A_storage <- A <- Receive request <- B
	 B_storage <- B <- Receive response <- A

	 The underlying logic is the same.

	 */

	class RawFrameBuffer {
		private:

			// A RawFrameBuffer should never contain any scope-related information.
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
			- Karkinolution::Networking::Frames::World::Protocol::Layout::TOTAL_HEADER_SIZE,
		MAX_RAW_FRAME_BUFFER_SIZE - Karkinolution::Networking::Frames::World::Protocol::Layout::TOTAL_HEADER_SIZE);

	namespace FrameBufferAnalyzer {
		enum class PlacesOfEndOfFrame : uint8_t {
			BUFFER,
			NEW_BYTES
		};

		struct EndOfFrame {
				std::size_t        index;
				PlacesOfEndOfFrame place;
		};

		BufferExistence has_frame(const RawFrameBuffer &buffer, Scope::Scope scope);
		BufferExistence has_frame(const std::vector<std::byte> &bytes, Scope::Scope);
		// get_size_of_frame only works when declared_size is set and depends solely on it.
		std::uint32_t get_size_of_frame(const RawFrameBuffer &buffer, Scope::Scope scope);
		std::uint32_t get_size_of_frame(const std::vector<std::byte> &declared_size, Scope::Scope scope);

		std::optional<std::vector<std::byte>> get_frame(const RawFrameBuffer &buffer, Scope::Scope scope);

		// This function only works when the Frame starts at an index within the RawFrameBuffer.
		std::optional<std::size_t> get_end_of_frame(const RawFrameBuffer &buffer, Scope::Scope scope);
		std::optional<std::size_t> get_end_of_frame(const std::vector<std::byte> &bytes, Scope::Scope scope);

		// This function works differently. It first checks whether the buffer contains a complete Frame, then checks
		// whether the newly received bytes complete the Frame as well. See the implementation for more details.
		std::optional<EndOfFrame>
		get_end_of_frame(const RawFrameBuffer &buffer, std::span<const std::byte> new_bytes, Scope::Scope scope);
	} // namespace FrameBufferAnalyzer

} // namespace Karkinolution::Networking
