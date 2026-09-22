#pragma once
#include <bit>
#include <cstddef>

namespace BiteUtils {
	inline std::byte char_to_byte(char byte) {
		return static_cast<std::byte>(static_cast<unsigned char>(byte));
	}

	inline unsigned int byte_to_int(std::byte byte) {
		return std::to_integer<unsigned int>(byte);
	}
} // namespace BiteUtils