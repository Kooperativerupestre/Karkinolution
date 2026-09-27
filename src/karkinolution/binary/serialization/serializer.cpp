#include <cstddef>
#include <karkinolution/binary/serialization/serializer.hpp>

using Serializer::Types::DoubleBytes;
using Serializer::Types::FloatBytes;
using Serializer::Types::StringBytes;
using Serializer::Types::Uint32tBytes;
using Serializer::Types::Uint64tBytes;
using Serializer::Types::Uint8tByte;

Uint32tBytes Serializer::convert_uint32_t(std::uint32_t value) {
	return Core::convert_4_bytes(value);
}

Uint8tByte Serializer::convert_uint8_t(std::uint8_t value) {
	return Core::convert_1_byte(value);
}

Uint64tBytes Serializer::convert_uint64_t(std::uint64_t value) {
	return Core::convert_8_bytes(value);
}

StringBytes Serializer::convert_string(const std::string &value) {
	std::vector<std::byte> bytes;
	bytes.reserve(value.size());

	for (char character : value) {
		bytes.push_back(static_cast<std::byte>(character));
	}

	return bytes;
}

DoubleBytes Serializer::convert_double(double value) {
	return Core::convert_8_bytes(std::bit_cast<std::uint64_t>(value));
}

FloatBytes Serializer::convert_float(float value) {
	return Core::convert_4_bytes(std::bit_cast<std::uint32_t>(value));
}

Serializer::Types::IntBytes Serializer::convert_int(int value) {
	return Core::convert_4_bytes(std::bit_cast<int>(value));
}