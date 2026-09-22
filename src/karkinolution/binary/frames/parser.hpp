#pragma once
#include <cstdint>
#include <deque>
#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <vector>

struct ParsedFrame {
		std::uint32_t                size;
		BinaryTypes                  type;
		BinarySubTypes::CodeSubTypes sub_type;
		std::vector<std::byte>       payload;
};

namespace FrameParser {
	template <ByteRange T> std::uint32_t get_size(const T &bytes) {
		return Deserializer::read_uint32_t(bytes, 0);
	}

	template <ByteRange T> BinaryTypes get_type(const T &bytes) {
		const auto raw = Deserializer::read_uint8_t(bytes, MESSAGE_SIZE_BYTES);

		BinaryValidator::validate_type(raw);
		return static_cast<BinaryTypes>(raw);
	}

	template <ByteRange T> BinarySubTypes::CodeSubTypes get_sub_type(const T &bytes) {
		const auto type = get_type(bytes);
		const auto read =
			Deserializer::read_uint32_t(bytes, MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES);

		BinaryValidator::validate_sub_type(type, read);

		if (type == BinaryTypes::Error) {

			return BinarySubTypes::Error{read};
		} else if (type == BinaryTypes::Request) {
			return BinarySubTypes::Request{read};
		} else if (type == BinaryTypes::Response) {
			return BinarySubTypes::Response{read};
		}
		throw ByteError("Unexpected error");
	}

	template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
		std::vector<std::byte> payload;

		const auto offset = MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES;

		payload.insert(payload.end(), bytes.begin() + offset, bytes.end());

		return payload;
	}

	template <ByteRange T> ParsedFrame parse_frame(const T &bytes) {
		return ParsedFrame{.size     = get_size(bytes),
						   .type     = get_type(bytes),
						   .sub_type = get_sub_type(bytes),
						   .payload  = get_payload(bytes)};
	}

} // namespace FrameParser
