#pragma once
#include <cstdint>
#include <variant>
/*

Protocol

size -> type -> sub type -> payload

[sizes](message_type_size.hpp)

The value of the field size means the number of bytes after the size field.
*/

enum class RequestResource : uint8_t {
	CREATURE = 1,
	EMBRYO   = 2,
	SOIL     = 3,
	CORPSE   = 4,
	EGG      = 5,
};


inline constexpr std::size_t MESSAGE_SUB_TYPE_BYTES = sizeof(std::uint8_t);  // bytes
inline constexpr std::size_t MESSAGE_TYPE_BYTES     = sizeof(std::uint8_t);  // bytes
inline constexpr std::size_t MESSAGE_SIZE_BYTES     = sizeof(std::uint32_t); // bytes
inline constexpr std::size_t REQUEST_RESOURCE_BYTES = sizeof(RequestResource);

namespace FrameOffsets {
	inline constexpr std::size_t TO_GET_SIZE     = 0;
	inline constexpr std::size_t TO_GET_TYPE     = TO_GET_SIZE + MESSAGE_SIZE_BYTES;
	inline constexpr std::size_t TO_GET_SUB_TYPE = TO_GET_TYPE + MESSAGE_TYPE_BYTES;
	inline constexpr std::size_t TO_GET_PAYLOAD  = TO_GET_SUB_TYPE + MESSAGE_SUB_TYPE_BYTES;

	inline constexpr std::size_t TO_GET_REQUEST_RESOURCE = TO_GET_PAYLOAD;
	inline constexpr std::size_t TO_GET_REQUEST_PAYLOAD =
		TO_GET_REQUEST_RESOURCE + REQUEST_RESOURCE_BYTES;

} // namespace FrameOffsets

inline constexpr std::size_t MESSAGE_HEADER_BYTES =
	MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES;


enum class BinaryTypes : uint8_t {
	Request  = 1,
	Error    = 2,
	Response = 3
};

namespace BinarySubTypes {
	enum class Request : uint8_t {
		/*
		 * # SEE
		 * similar to get, can return nothing if there isn't
		 *
		 * # WANT
		 * this action is used when you want to see a different state in creature. For example
		 *
		 * WANT Creature pregnant
		 * WANT Creature dead
		 *
		 * (and the change only will be executed if the system allows)
		 *
		 * # ERASE
		 *
		 * delete the resource without respecting the laws
		 *
		 * # CREATE
		 *
		 * create the resource
		 *
		 * # CYCLE
		 *
		 * run a tick on the resource

		 */
		SEE    = 1,
		WANT   = 2,
		ERASE  = 3,
		CREATE = 4,
		CYCLE  = 5
	};


	enum class Error : uint8_t {
		CREATURE_WAS_NOT_FOUND = 1,
		CORPSE_WAS_NOT_FOUND   = 2,
	};

	enum class Response : uint8_t {
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
