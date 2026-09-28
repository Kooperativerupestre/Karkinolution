#pragma once
#include <cstdint>
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
		GET_CORPSE   = 2,
	};


	enum class Error : uint32_t {
		CREATURE_WAS_NOT_FOUND = 1,
		CORPSE_WAS_NOT_FOUND   = 2,
	};

	enum class Response : uint32_t {
		CREATURE = 0,
		CORPSE   = 1,
	};

	using CodeSubTypes = std::variant<Request, Error, Response>;

} // namespace BinarySubTypes

static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

static_assert(sizeof(int) == 4);
static_assert(sizeof(long long) == 8);

enum class BinaryNLT : uint8_t {
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
