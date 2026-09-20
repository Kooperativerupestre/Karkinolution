#include "karkinolution/binary/message_type_size.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <utility>
#include <variant>

std::uint32_t FrameParser::get_size(const std::vector<std::byte> &bytes) {
	return Deserializer::read_uint32_t(bytes, 0);
}

BinaryTypes FrameParser::get_type(const std::vector<std::byte> &bytes) {
	return BinaryTypes(Deserializer::read_uint8_t(bytes, MESSAGE_SIZE_BYTES));
}

BinarySubTypes::CodeSubTypes FrameParser::get_sub_type(const std::vector<std::byte> &bytes) {
	const auto type = get_type(bytes);
	const auto read = Deserializer::read_uint32_t(bytes, MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES);

	if (type == BinaryTypes::Error) {
		return BinarySubTypes::Error{read};
	} else if (type == BinaryTypes::Request) {
		return BinarySubTypes::Request{read};
	} else if (type == BinaryTypes::Response) {
		return BinarySubTypes::Response{read};
	}

	std::unreachable();
}

std::vector<std::byte> FrameParser::get_payload(const std::vector<std::byte> &bytes) {
	std::vector<std::byte> payload;

	const auto offset = MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES;

	payload.insert(payload.end(), bytes.begin() + offset, bytes.end());

	return payload;
}

std::uint32_t FrameParser::get_size(const std::deque<std::byte> &bytes) {
	return Deserializer::read_uint32_t(bytes, 0);
}

BinaryTypes FrameParser::get_type(const std::deque<std::byte> &bytes) {
	return BinaryTypes(Deserializer::read_uint8_t(bytes, MESSAGE_SIZE_BYTES));
}

BinarySubTypes::CodeSubTypes FrameParser::get_sub_type(const std::deque<std::byte> &bytes) {
	const auto code = get_sub_type(bytes);
	const auto read =
		Deserializer::read_uint32_t(bytes, MESSAGE_SIZE_BYTES + MESSAGE_SUB_TYPE_BYTES);

	if (std::holds_alternative<BinarySubTypes::Error>(code)) {
		return BinarySubTypes::Error{read};
	} else if (std::holds_alternative<BinarySubTypes::Request>(code)) {
		return BinarySubTypes::Request{read};
	} else if (std::holds_alternative<BinarySubTypes::Response>(code)) {
		return BinarySubTypes::Response{read};
	}

	std::unreachable();
}

std::vector<std::byte> FrameParser::get_payload(const std::deque<std::byte> &bytes) {
	std::vector<std::byte> payload;

	const auto offset = MESSAGE_SIZE_BYTES + MESSAGE_SUB_TYPE_BYTES + MESSAGE_TYPE_BYTES;

	payload.insert(payload.end(), bytes.begin() + offset, bytes.end());

	return payload;
}

ParsedFrame FrameParser::parse_frame(const std::deque<std::byte> &bytes) {
	return ParsedFrame{.size     = get_size(bytes),
					   .type     = get_type(bytes),
					   .sub_type = get_sub_type(bytes),
					   .payload  = get_payload(bytes)};
}

ParsedFrame FrameParser::parse_frame(const std::vector<std::byte> &bytes) {
	return ParsedFrame{.size     = get_size(bytes),
					   .type     = get_type(bytes),
					   .sub_type = get_sub_type(bytes),
					   .payload  = get_payload(bytes)};
}
