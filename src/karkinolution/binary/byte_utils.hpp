#pragma once
#include <bit>
#include <cstddef>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/core/error.hpp>

namespace ByteUtils {
	inline std::byte char_to_byte(char byte) {
		return static_cast<std::byte>(static_cast<unsigned char>(byte));
	}

	inline unsigned int byte_to_int(std::byte byte) {
		return std::to_integer<unsigned int>(byte);
	}
} // namespace ByteUtils

namespace BinaryNLTUtils {
	template <typename T> constexpr BinaryNLT get() {
		if constexpr (std::is_same<T, float>()) {
			return BinaryNLT::FLOAT;
		} else if constexpr (std::is_same<T, double>()) {
			return BinaryNLT::DOUBLE;
		} else if constexpr (std::is_same<T, uint8_t>()) {
			return BinaryNLT::UINT8_T;
		} else if constexpr (std::is_same<T, uint32_t>()) {
			return BinaryNLT::UINT32_T;
		} else if constexpr (std::is_same<T, uint64_t>()) {
			return BinaryNLT::UINT64_T;
		} else if constexpr (std::is_same<T, int>()) {
			return BinaryNLT::INT;
		}

		throw ByteError(
			"Type doesn't exists as dynamic numeric language (binary) type of karkinolution");
	}
} // namespace BinaryNLTUtils