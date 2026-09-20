
#include <cassert>
#include <cstddef>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/message_type_size.hpp>

std::uint32_t Deserializer::read_uint32_t(const std::vector<std::byte> &bytes, std::size_t offset) {

	assert(bytes.size() > offset + 3);
	return (std::to_integer<std::uint32_t>(bytes[offset]) << 24)
		| (std::to_integer<std::uint32_t>(bytes[offset + 1]) << 16)
		| (std::to_integer<std::uint32_t>(bytes[offset + 2]) << 8)
		| std::to_integer<std::uint32_t>(bytes[offset + 3]);
}

std::uint8_t Deserializer::read_uint8_t(const std::vector<std::byte> &bytes, std::size_t offset) {
	assert(bytes.size() > offset);
	return std::to_integer<std::uint8_t>(bytes[offset]);
}

std::uint64_t Deserializer::read_uint64_t(const std::vector<std::byte> &bytes, std::size_t offset) {

	assert(bytes.size() > offset + 7);
	return (std::to_integer<std::uint64_t>(bytes[offset + 0]) << 56)
		| (std::to_integer<std::uint64_t>(bytes[offset + 1]) << 48)
		| (std::to_integer<std::uint64_t>(bytes[offset + 2]) << 40)
		| (std::to_integer<std::uint64_t>(bytes[offset + 3]) << 32)
		| (std::to_integer<std::uint64_t>(bytes[offset + 4]) << 24)
		| (std::to_integer<std::uint64_t>(bytes[offset + 5]) << 16)
		| (std::to_integer<std::uint64_t>(bytes[offset + 6]) << 8)
		| (std::to_integer<std::uint64_t>(bytes[offset + 7]));
}

std::string Deserializer::read_string(const std::vector<std::byte> &bytes,
									  std::size_t                   offset,
									  std::size_t                   length) {
	std::string result;

	result.reserve(length);

	for (std::size_t i = 0; i < length; i++) {
		result.push_back(std::to_integer<char>(bytes[offset + i]));
	}
	return result;
}

void Deserializer::append_bytes(std::vector<std::byte>       &bytes,
								const std::vector<std::byte> &value) {
	for (const auto &byte : value) {
		bytes.push_back(byte);
	}
}

void Deserializer::append_bytes(std::vector<std::byte> &bytes, std::byte value) {
	bytes.push_back(value);
}

std::uint32_t Deserializer::read_uint32_t(const std::deque<std::byte> &bytes, std::size_t offset) {
	assert(bytes.size() > offset + 3);
	return (std::to_integer<std::uint32_t>(bytes[offset]) << 24)
		| (std::to_integer<std::uint32_t>(bytes[offset + 1]) << 16)
		| (std::to_integer<std::uint32_t>(bytes[offset + 2]) << 8)
		| std::to_integer<std::uint32_t>(bytes[offset + 3]);
}

std::uint8_t Deserializer::read_uint8_t(const std::deque<std::byte> &bytes, std::size_t offset) {
	assert(bytes.size() > offset);
	return std::to_integer<std::uint8_t>(bytes[offset]);
}

std::uint64_t Deserializer::read_uint64_t(const std::deque<std::byte> &bytes, std::size_t offset) {
	assert(bytes.size() > offset + 7);
	return (std::to_integer<std::uint64_t>(bytes[offset + 0]) << 56)
		| (std::to_integer<std::uint64_t>(bytes[offset + 1]) << 48)
		| (std::to_integer<std::uint64_t>(bytes[offset + 2]) << 40)
		| (std::to_integer<std::uint64_t>(bytes[offset + 3]) << 32)
		| (std::to_integer<std::uint64_t>(bytes[offset + 4]) << 24)
		| (std::to_integer<std::uint64_t>(bytes[offset + 5]) << 16)
		| (std::to_integer<std::uint64_t>(bytes[offset + 6]) << 8)
		| (std::to_integer<std::uint64_t>(bytes[offset + 7]));
}

std::string Deserializer::read_string(const std::deque<std::byte> &bytes,
									  std::size_t                  offset,
									  std::size_t                  length) {
	std::string result;

	result.reserve(length);

	for (std::size_t i = 0; i < length; i++) {
		result.push_back(std::to_integer<char>(bytes[offset + i]));
	}

	return result;
}

void Deserializer::append_bytes(std::deque<std::byte> &bytes, const std::vector<std::byte> &value) {
	for (const auto &byte : value) {
		bytes.push_back(byte);
	}
}

void Deserializer::append_bytes(std::deque<std::byte> &bytes, std::byte value) {
	bytes.push_back(value);
}
