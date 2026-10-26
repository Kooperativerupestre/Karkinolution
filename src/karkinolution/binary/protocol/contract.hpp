#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>

#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>

namespace Karkinolution::Binary::Protocol {

	/*
	# Base Protocol

	size -> type -> sub type

	## Request types

	base -> request id -> resource -> payload

	## Response types

	base -> request id -> payload

	## Error types

	base -> request id -> payload

	## Event types

	base -> payload

	# Note

	The value of the field size means the number of bytes
	after the size field.
	*/

	enum class Type : uint8_t {
		Request  = 1,
		Error    = 2,
		Response = 3
	};

	enum class Resource : uint8_t {
		CREATURE = 1,
		EMBRYO   = 2,
		SOIL     = 3,
		CORPSE   = 4,
		EGG      = 5
	};

	enum class Operation : uint8_t {
		SEE    = 1,
		WANT   = 2,
		ERASE  = 3,
		CREATE = 4,
		CYCLE  = 5
	};

	/*
	# SEE

	Similar to GET.

	Can return NONE if the requested resource does not exist.
	SEE does not send ERROR.

	Examples:

	SEE CREATURE <id>
	SEE ALL SOIL

	# WANT

	This action is used when you want to see a different state
	in a creature.

	Examples:

	WANT CREATURE PREGNANT
	WANT CREATURE DEAD

	The change is only executed if the system allows it.

	# ERASE

	Delete the resource without respecting the laws.

	# CREATE

	Create the resource.

	# CYCLE

	Run a tick on the resource.
	*/

	enum class ErrorCode : uint8_t {
		CREATURE_WAS_NOT_FOUND = 1,
		CORPSE_WAS_NOT_FOUND   = 2
	};

	enum class ResponseCode : uint8_t {
		CREATURE = 0,
		CORPSE   = 1,
		NONE     = 3,
		SOIL     = 4
	};

	enum class NumericType : uint8_t {
		FLOAT  = 1,
		DOUBLE = 2,
		UINT8  = 3,
		UINT16 = 4,
		UINT32 = 5,
		UINT64 = 6,
		INT    = 7
	};

	namespace Detail {

		template <typename T> using UnderlyingType = std::underlying_type_t<T>;

		using CodeVariant = std::variant<Operation, ErrorCode, ResponseCode>;

		using ResponseVariant = std::variant<ErrorCode, ResponseCode>;

	} // namespace Detail

} // namespace Karkinolution::Binary::Protocol
