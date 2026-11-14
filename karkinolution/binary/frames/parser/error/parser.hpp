#pragma once
#include <cstdint>
#include <karkinolution/binary/frames/parser/internal.hpp>

namespace Karkinolution::Binary::Frames::Parser {
	namespace Error {

		constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
			return Internal::calculate_declared_size(payload_size, ProtocolLayout::Error::Payload::OFFSET);
		}

		constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
			return Internal::calculate_payload_size(declared_size, ProtocolLayout::Error::Payload::OFFSET);
		}

		template <ByteRange T> constexpr std::uint32_t calculate_payload_size(const T &bytes) {
			return Internal::calculate_payload_size(bytes, ProtocolLayout::Error::Payload::OFFSET);
		}

		template <ByteRange T> RequestId get_request_id(const T &bytes) {
			return Karkinolution::Binary::Deserializer::deserialize<RequestId>(
				bytes,
				ProtocolLayout::Error::RequestId::OFFSET);
		}

		template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
			std::vector<std::byte> payload;

			payload.insert(payload.end(),
						   bytes.begin() + ProtocolLayout::Error::Payload::OFFSET,
						   bytes.begin() + ProtocolLayout::Error::Payload::OFFSET
							   + Internal::calculate_payload_size(bytes, ProtocolLayout::Error::Payload::OFFSET));
			return payload;
		}

	} // namespace Error
} // namespace Karkinolution::Binary::Frames::Parser