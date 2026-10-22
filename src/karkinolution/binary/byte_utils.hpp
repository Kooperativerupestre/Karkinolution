#pragma once
#include "byte_range.hpp"

#include <bit>
#include <cstddef>
#include <deque>
#include <format>
#include <karkinolution/binary/protocol/aliases.hpp>
#include <karkinolution/binary/protocol/contract.hpp>
#include <karkinolution/core/error.hpp>
#include <vector>

namespace BinaryUtils {
	inline std::byte char_to_byte(char byte) {
		return static_cast<std::byte>(static_cast<unsigned char>(byte));
	}

	inline unsigned int byte_to_int(std::byte byte) {
		return std::to_integer<unsigned int>(byte);
	}

	template <ByteRange T> constexpr void zero_padding(T &payload, std::size_t offset) {
		if (offset > payload.size()) {
			throw ByteError(
				std::format("Failed to apply zero padding on array that has {} size with offset {}",
							payload.size(),
							offset));
		}

		for (std::size_t i = offset; i < payload.size(); ++i) {
			payload[i] = std::byte{0};
		}
	}

	uint8_t constexpr cast_numeric(
		Karkinolution::Binary::Protocol::Types::RequestResource resource) {
		return static_cast<uint8_t>(resource);
	}

	uint8_t constexpr cast_numeric(Karkinolution::Binary::Protocol::Types::BinaryTypes type) {
		return static_cast<uint8_t>(type);
	}

	uint8_t constexpr cast_numeric(
		Karkinolution::Binary::Protocol::Types::BinarySubTypes::CodeSubTypes sub_type) {
		using namespace Karkinolution::Binary::Protocol::Types::BinarySubTypes;
		if (std::holds_alternative<Request>(sub_type)) {
			return static_cast<uint8_t>(std::get<Request>(sub_type));
		} else if (std::holds_alternative<Error>(sub_type)) {
			return static_cast<uint8_t>(std::get<Error>(sub_type));
		} else if (std::holds_alternative<Response>(sub_type)) {
			return static_cast<uint8_t>(std::get<Response>(sub_type));
		}
		throw ByteError("Invalid sub type");
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

	template <ByteRange T>
	void append_bytes(std::vector<std::byte> &bytes, const T &value, std::size_t offset) {
		for (std::size_t i = 0; i < value.size(); i++) {
			bytes[offset + i] = value[i];
		}
	}

	inline void append_bytes(std::deque<std::byte> &bytes, std::byte value) {
		bytes.push_back(value);
	}

	template <std::size_t Size>
	void append_bytes(std::array<std::byte, Size> &bytes, std::byte value, std::size_t offset) {
		bytes[offset] = value;
	}

} // namespace BinaryUtils

namespace BinaryNLTUtils {
	template <typename T> constexpr Karkinolution::Binary::Protocol::Types::BinaryNLT get() {
		using Karkinolution::Binary::Protocol::Types::BinaryNLT;
		if constexpr (std::is_same_v<T, float>) {
			return BinaryNLT::FLOAT;
		} else if constexpr (std::is_same_v<T, double>) {
			return BinaryNLT::DOUBLE;
		} else if constexpr (std::is_same_v<T, uint8_t>) {
			return BinaryNLT::UINT8_T;
		} else if constexpr (std::is_same_v<T, uint32_t>) {
			return BinaryNLT::UINT32_T;
		} else if constexpr (std::is_same_v<T, uint64_t>) {
			return BinaryNLT::UINT64_T;
		} else if constexpr (std::is_same_v<T, int>) {
			return BinaryNLT::INT;
		}

		throw ByteError(
			"Type doesn't exists as dynamic numeric language (binary) type of karkinolution");
	}
} // namespace BinaryNLTUtils
