#pragma once
#include <cstdint>
#include <format>
#include <karkinolution/core/error.hpp>
#include <variant>
/*

Protocol

size -> type -> sub type -> payload

[sizes](message_type_size.hpp)

The value of the field size means the number of bytes after the size field.
*/

inline constexpr std::size_t MESSAGE_SUB_TYPE_BYTES = sizeof(std::uint32_t); // bytes
inline constexpr std::size_t MESSAGE_TYPE_BYTES     = sizeof(std::uint8_t);  // bytes
inline constexpr std::size_t MESSAGE_SIZE_BYTES     = sizeof(std::uint32_t); // bytes

inline constexpr std::size_t MESSAGE_HEADER_BYTES =
	MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES;


enum class BinaryTypes : uint8_t {
	Request  = 1,
	Error    = 2,
	Response = 3
};

namespace BinarySubTypes {
	enum class Request : uint32_t {
		GET_CREATURE = 1,
	};


	enum class Error : uint32_t {
		CREATURE_WAS_NOT_FOUND = 1
	};

	enum class Response : uint32_t {
		CREATURE
	};

	using CodeSubTypes = std::variant<Request, Error, Response>;

} // namespace BinarySubTypes

static_assert(sizeof(double) == 8);
static_assert(std::numeric_limits<double>::is_iec559);

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