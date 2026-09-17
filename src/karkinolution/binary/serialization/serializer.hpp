#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

using Uint32tBytes = std::array<std::byte, sizeof(std::uint32_t)>;
using Uint64tBytes = std::array<std::byte, sizeof(std::uint64_t)>;
using StringBytes  = std::vector<std::byte>;
using Uint8tByte   = std::byte;

namespace Serializer {
	Uint32tBytes convert_uint32_t(std::uint32_t value);
	Uint64tBytes convert_uint64_t(std::uint64_t value);
	Uint8tByte   convert_uint8_t(std::uint8_t value);

	StringBytes convert_string(const std::string &string);
} // namespace Serializer