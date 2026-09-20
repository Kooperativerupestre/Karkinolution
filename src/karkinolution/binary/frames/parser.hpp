#pragma once
#include <cstdint>
#include <deque>
#include <karkinolution/binary/message_type_size.hpp>
#include <vector>

struct ParsedFrame {
		std::uint32_t                size;
		BinaryTypes                  type;
		BinarySubTypes::CodeSubTypes sub_type;
		std::vector<std::byte>       payload;
};

namespace FrameParser {
	std::uint32_t get_size(const std::vector<std::byte> &bytes);
	std::uint32_t get_size(const std::deque<std::byte> &bytes);

	BinaryTypes get_type(const std::vector<std::byte> &bytes);
	BinaryTypes get_type(const std::deque<std::byte> &bytes);

	BinarySubTypes::CodeSubTypes get_sub_type(const std::vector<std::byte> &bytes);
	BinarySubTypes::CodeSubTypes get_sub_type(const std::deque<std::byte> &bytes);

	std::vector<std::byte> get_payload(const std::deque<std::byte> &bytes);
	std::vector<std::byte> get_payload(const std::vector<std::byte> &bytes);

	ParsedFrame parse_frame(const std::deque<std::byte> &bytes);
	ParsedFrame parse_frame(const std::vector<std::byte> &bytes);
} // namespace FrameParser
