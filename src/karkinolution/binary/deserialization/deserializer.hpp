#pragma once


#include <cassert>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <format>
#include <karkinolution/binary/binary_error.hpp>
#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/core/error.hpp>
#include <string>
#include <vector>

namespace Deserializer {

	template <ByteRange T> std::uint32_t read_uint32_t(const T &bytes, std::size_t offset) {
		if (bytes.size() < offset + 4) {
			throw ByteError(BinaryErrorFactory::failed_conversion(
				std::format("the size of bytes ({}) are lower than required {}",
							bytes.size(),
							offset),
				"uint32_t"));
		}
		return (std::to_integer<std::uint32_t>(bytes[offset]) << 24)
			| (std::to_integer<std::uint32_t>(bytes[offset + 1]) << 16)
			| (std::to_integer<std::uint32_t>(bytes[offset + 2]) << 8)
			| std::to_integer<std::uint32_t>(bytes[offset + 3]);
	}

	template <ByteRange T> std::uint64_t read_uint64_t(const T &bytes, std::size_t offset) {
		if (bytes.size() < offset + 8) {
			throw ByteError(BinaryErrorFactory::failed_conversion(
				std::format("the size of bytes ({}) are lower than required {}",
							bytes.size(),
							offset),
				"uint64_t"));
		}
		return (std::to_integer<std::uint64_t>(bytes[offset]) << 56)
			| (std::to_integer<std::uint64_t>(bytes[offset + 1]) << 48)
			| (std::to_integer<std::uint64_t>(bytes[offset + 2]) << 40)
			| (std::to_integer<std::uint64_t>(bytes[offset + 3]) << 32)
			| (std::to_integer<std::uint64_t>(bytes[offset + 4]) << 24)
			| (std::to_integer<std::uint64_t>(bytes[offset + 5]) << 16)
			| (std::to_integer<std::uint64_t>(bytes[offset + 6]) << 8)
			| std::to_integer<std::uint64_t>(bytes[offset + 7]);
	}

	template <ByteRange T> double read_double(const T &bytes, std::size_t offset) {
		if (bytes.size() < offset + sizeof(double)) {
			throw ByteError(BinaryErrorFactory::failed_conversion(
				std::format("the size of bytes ({}) is lower than required {}",
							bytes.size(),
							offset),
				"double"));
		}


		const auto bits = read_uint64_t(bytes, offset);

		return std::bit_cast<double>(bits);
	}

	template <ByteRange T> std::uint8_t read_uint8_t(const T &bytes, std::size_t offset) {
		if (bytes.size() <= offset) {
			throw ByteError(BinaryErrorFactory::failed_conversion(
				std::format("the size of bytes ({}) is lower than required {}",
							bytes.size(),
							offset),
				"uint8_t"));
		}

		return std::to_integer<std::uint8_t>(bytes[offset]);
	}

	template <ByteRange T>
	std::string read_string(const T &bytes, std::size_t offset, std::size_t length) {
		if (bytes.size() < offset + length) {
			throw ByteError(BinaryErrorFactory::failed_conversion(
				std::format("the size of bytes ({}) is lower than required {}",
							bytes.size(),
							offset),
				"string"));
		}
		std::string result;
		result.reserve(length);

		for (std::size_t i = 0; i < length; ++i) {
			result.push_back(std::to_integer<char>(bytes[offset + i]));
		}

		return result;
	}

	template <ByteRange T> void append_bytes(std::vector<std::byte> &bytes, const T &value) {
		for (const auto &byte : value) {
			bytes.push_back(byte);
		}
	}

	inline void append_bytes(std::vector<std::byte> &bytes, std::byte value) {
		bytes.push_back(value);
	}

	template <ByteRange T> void append_bytes(std::deque<std::byte> &bytes, const T &value) {
		for (const auto &byte : value) {
			bytes.push_back(byte);
		}
	}

	template <ByteRange T, size_t Size>
	void append_bytes(std::array<std::byte, Size> &bytes, const T &value, size_t offset) {
		for (size_t i = 0; i < value.size(); i++) {
			bytes[offset + i] = value[i];
		}
	}

	inline void append_bytes(std::deque<std::byte> &bytes, std::byte value) {
		bytes.push_back(value);
	}


} // namespace Deserializer