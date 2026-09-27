#pragma once
#include <cstdint>
#include <format>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/core/error.hpp>
#include <string>

namespace BinaryValidator {
	constexpr void validate_type(uint8_t value) {
		if (value != static_cast<uint8_t>(BinaryTypes::Request)
			&& value != static_cast<uint8_t>(BinaryTypes::Response)
			&& value != static_cast<uint8_t>(BinaryTypes::Error)) {
			throw ByteError(std::format("Invalid frame type: {}", value));
		}
	}

	constexpr void validate_error_sub_type(uint32_t value) {
		if (value != static_cast<uint32_t>(BinarySubTypes::Error::CREATURE_WAS_NOT_FOUND)) {
			throw ByteError(std::format("Invalid frame sub type: {}", value));
		}
	}

	constexpr void validate_request_sub_type(uint32_t value) {
		if (value != static_cast<uint32_t>(BinarySubTypes::Request::GET_CREATURE)) {
			throw ByteError(std::format("Invalid frame sub type: {}", value));
		}
	}

	constexpr void validate_response_sub_type(uint32_t value) {
		if (value != static_cast<uint32_t>(BinarySubTypes::Response::CREATURE)) {
			throw ByteError(std::format("Invalid frame sub type: {}", value));
		}
	}

	constexpr void validate_sub_type(BinaryTypes type, uint32_t value) {
		if (type == BinaryTypes::Request) {
			validate_request_sub_type(value);
		} else if (type == BinaryTypes::Response) {
			validate_response_sub_type(value);
		} else if (type == BinaryTypes::Error) {
			validate_error_sub_type(value);
		} else {
			throw ByteError(std::format("Invalid type: {}", static_cast<uint8_t>(type)));
		}
	}


} // namespace BinaryValidator

namespace BinaryNLTValidator {
	template <typename T> constexpr void validate_dynamic_type_exists() {
		if (!(std::is_same<T, float>() || std::is_same<T, double>() || std::is_same<T, uint8_t>()
			  || std::is_same<T, uint32_t>() || std::is_same<T, uint16_t>()
			  || std::is_same<T, uint64_t>())) {
			throw ByteError(
				"Type doesn't exists as dynamic numeric language (binary) type of karkinolution");
		}
	}
} // namespace BinaryNLTValidator