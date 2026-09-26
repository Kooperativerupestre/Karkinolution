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
static_assert(sizeof(float) == 4);
static_assert(std::numeric_limits<double>::is_iec559);


enum class BinaryNumericLanguageTypes : uint8_t {
	FLOAT    = 1,
	DOUBLE   = 2,
	UINT8_T  = 3,
	UINT16_T = 4,
	UINT32_T = 5,
	UINT64_T = 6,
	INT      = 7,
};

/*
 * This enum should be used in the protocol _whenever_ the type of something is dynamic
 * For example: LimitedValue.
 * The size of the fields that have the dynamic type should have the maximum possible size.
 */

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

	template <typename T> constexpr void validate_dynamic_type_exists() {
		if (!(std::is_same<T, float>() || std::is_same<T, double>() || std::is_same<T, uint8_t>()
			  || std::is_same<T, uint32_t>() || std::is_same<T, uint16_t>()
			  || std::is_same<T, uint64_t>())) {
			throw ByteError(
				"Type doesn't exists as dynamic numeric language (binary) type of karkinolution");
		}
	}
} // namespace BinaryValidator

namespace BinaryNumericLanguageTypesUtils {
	template <typename T> constexpr BinaryNumericLanguageTypes get() {
		if constexpr (std::is_same<T, float>()) {
			return BinaryNumericLanguageTypes::FLOAT;
		} else if constexpr (std::is_same<T, double>()) {
			return BinaryNumericLanguageTypes::DOUBLE;
		} else if constexpr (std::is_same<T, uint8_t>()) {
			return BinaryNumericLanguageTypes::UINT8_T;
		} else if constexpr (std::is_same<T, uint32_t>()) {
			return BinaryNumericLanguageTypes::UINT32_T;
		} else if constexpr (std::is_same<T, uint64_t>()) {
			return BinaryNumericLanguageTypes::UINT64_T;
		} else if constexpr (std::is_same<T, int>()) {
			return BinaryNumericLanguageTypes::INT;
		}

		throw ByteError(
			"Type doesn't exists as dynamic numeric language (binary) type of karkinolution");
	}
} // namespace BinaryNumericLanguageTypesUtils