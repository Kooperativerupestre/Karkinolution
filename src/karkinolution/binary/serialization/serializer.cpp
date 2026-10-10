#include <cstddef>
#include <karkinolution/binary/serialization/serializer.hpp>

using Serializer::Core::EightBytes;
using Serializer::Core::FourBytes;
using Serializer::Core::TwoBytes;

FourBytes Serializer::convert_uint32_t(std::uint32_t value) {
	return Core::convert_4_bytes(value);
}

std::byte Serializer::convert_uint8_t(std::uint8_t value) {
	return Core::convert_1_byte(value);
}

EightBytes Serializer::convert_uint64_t(std::uint64_t value) {
	return Core::convert_8_bytes(value);
}

std::vector<std::byte> Serializer::convert_string(const std::string &value) {
	std::vector<std::byte> bytes;
	bytes.reserve(value.size());

	for (char character : value) {
		bytes.push_back(static_cast<std::byte>(character));
	}

	return bytes;
}

FourBytes Serializer::convert_int(int value) {
	return Core::convert_4_bytes(std::bit_cast<std::uint32_t>(value));
}

EightBytes Serializer::convert_double(double value) {
	return Core::convert_8_bytes(std::bit_cast<std::uint64_t>(value));
}

FourBytes Serializer::convert_float(float value) {
	return Core::convert_4_bytes(std::bit_cast<std::uint32_t>(value));
}

std::byte Serializer::convert_int8_t(std::int8_t value) {
	return Core::convert_1_byte(value);
}

TwoBytes Serializer::convert_int16_t(std::int16_t value) {
	return Core::convert_2_bytes(value);
}

FourBytes Serializer::convert_int32_t(std::int32_t value) {
	return Core::convert_4_bytes(value);
}

EightBytes Serializer::convert_int64_t(std::int64_t value) {
	return Core::convert_8_bytes(value);
}

std::byte Serializer::convert_bool(bool value) {
	if (value) {
		return std::byte(0x01);
	} else {
		return std::byte(0x00);
	}
}