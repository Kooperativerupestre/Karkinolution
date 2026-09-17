#include <cstddef>
#include <karkinolution/binary/serialization/serializer.hpp>

Uint32tBytes Serializer::convert_uint32_t(std::uint32_t value) {
	Uint32tBytes bytes;

	bytes[0] = std::byte(value >> 24 & 0xFF);
	bytes[1] = std::byte(value >> 16 & 0xFF);
	bytes[2] = std::byte(value >> 8 & 0xFF);
	bytes[3] = std::byte(value & 0xFF);
	return bytes;
}

Uint8tByte Serializer::convert_uint8_t(std::uint8_t value) {
	return std::byte(value);
}

Uint64tBytes Serializer::convert_uint64_t(std::uint64_t value) {
	Uint64tBytes bytes;

	bytes[0] = std::byte(value >> 56 & 0xFF);
	bytes[1] = std::byte(value >> 48 & 0xFF);
	bytes[2] = std::byte(value >> 40 & 0xFF);
	bytes[3] = std::byte(value >> 32 & 0xFF);
	bytes[4] = std::byte(value >> 24 & 0xFF);
	bytes[5] = std::byte(value >> 16 & 0xFF);
	bytes[6] = std::byte(value >> 8 & 0xFF);
	bytes[7] = std::byte(value & 0xFF);
	return bytes;
}

StringBytes Serializer::convert_string(const std::string &value) {
	std::vector<std::byte> bytes;
	bytes.reserve(value.size());

	for (char character : value) {
		bytes.push_back(static_cast<std::byte>(character));
	}

	return bytes;
}