#pragma once
#include "byte_range.hpp"

#include <bit>
#include <cstddef>
#include <format>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/core/error.hpp>

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

	uint8_t constexpr cast_numeric(Frames::Types::RequestResource resource) {
		return static_cast<uint8_t>(resource);
	}

	uint8_t constexpr cast_numeric(Frames::Types::BinaryTypes type) {
		return static_cast<uint8_t>(type);
	}

	uint8_t constexpr cast_numeric(Frames::Types::BinarySubTypes::CodeSubTypes sub_type) {
		if (std::holds_alternative<Frames::Types::BinarySubTypes::Request>(sub_type)) {
			return static_cast<uint8_t>(std::get<Frames::Types::BinarySubTypes::Request>(sub_type));
		} else if (std::holds_alternative<Frames::Types::BinarySubTypes::Error>(sub_type)) {
			return static_cast<uint8_t>(std::get<Frames::Types::BinarySubTypes::Error>(sub_type));
		} else if (std::holds_alternative<Frames::Types::BinarySubTypes::Response>(sub_type)) {
			return static_cast<uint8_t>(
				std::get<Frames::Types::BinarySubTypes::Response>(sub_type));
		}
		throw ByteError("Invalid sub type");
	}


} // namespace BinaryUtils

namespace BinaryNLTUtils {
	template <typename T> constexpr Frames::Types::BinaryNLT get() {
		if constexpr (std::is_same<T, float>()) {
			return Frames::Types::BinaryNLT::FLOAT;
		} else if constexpr (std::is_same<T, double>()) {
			return Frames::Types::BinaryNLT::DOUBLE;
		} else if constexpr (std::is_same<T, uint8_t>()) {
			return Frames::Types::BinaryNLT::UINT8_T;
		} else if constexpr (std::is_same<T, uint32_t>()) {
			return Frames::Types::BinaryNLT::UINT32_T;
		} else if constexpr (std::is_same<T, uint64_t>()) {
			return Frames::Types::BinaryNLT::UINT64_T;
		} else if constexpr (std::is_same<T, int>()) {
			return Frames::Types::BinaryNLT::INT;
		}

		throw ByteError(
			"Type doesn't exists as dynamic numeric language (binary) type of karkinolution");
	}
} // namespace BinaryNLTUtils