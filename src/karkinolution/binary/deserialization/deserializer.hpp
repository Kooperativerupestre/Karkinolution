#pragma once


#include <cstdint>
#include <deque>
#include <string>
#include <vector>

namespace Deserializer {
	std::uint32_t read_uint32_t(const std::vector<std::byte> &bytes, std::size_t offset);
	std::uint64_t read_uint64_t(const std::vector<std::byte> &bytes, std::size_t offset);
	std::uint8_t  read_uint8_t(const std::vector<std::byte> &bytes, std::size_t offset);
	std::string
	read_string(const std::vector<std::byte> &bytes, std::size_t offset, std::size_t length);

	std::uint32_t read_uint32_t(const std::deque<std::byte> &bytes, std::size_t offset);
	std::uint64_t read_uint64_t(const std::deque<std::byte> &bytes, std::size_t offset);
	std::uint8_t  read_uint8_t(const std::deque<std::byte> &bytes, std::size_t offset);
	std::string
	read_string(const std::deque<std::byte> &bytes, std::size_t offset, std::size_t length);

	// utility function. It doesn't have logic

	void append_bytes(std::vector<std::byte> &bytes, const std::vector<std::byte> &value);

	template <size_t size>
	void append_bytes(std::vector<std::byte> &bytes, const std::array<std::byte, size> &value) {
		for (size_t i = 0; i < size; i++) {
			bytes.push_back(value[i]);
		}
	}

	void append_bytes(std::vector<std::byte> &bytes, std::byte value);

	void append_bytes(std::deque<std::byte> &bytes, const std::vector<std::byte> &value);

	template <size_t size>
	void append_bytes(std::deque<std::byte> &bytes, const std::array<std::byte, size> &value) {
		for (size_t i = 0; i < size; i++) {
			bytes.push_back(value[i]);
		}
	}

	void append_bytes(std::deque<std::byte> &bytes, const std::deque<std::byte> &value);

	void append_bytes(std::deque<std::byte> &bytes, std::byte value);

} // namespace Deserializer