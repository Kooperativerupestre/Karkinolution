#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>

/*
# Base Protocol

size -> type -> sub type

## Request types

### Request

base -> id -> resource -> payload

## Response types

### Response

base -> request id -> payload

### Error

base -> request id -> payload

### Event

base -> payload

# Note
The value of the field size means the number of bytes after the size field.
*/

namespace Karkinolution::Binary::Protocol {

	namespace Types {
		template <typename T> using UnderlyingType = std::underlying_type_t<T>;

		enum class RequestResource : uint8_t {
			CREATURE = 1,
			EMBRYO   = 2,
			SOIL     = 3,
			CORPSE   = 4,
			EGG      = 5,
		};

		using RequestResourceEnumType = UnderlyingType<RequestResource>;

		enum class BinaryTypes : uint8_t {
			Request  = 1,
			Error    = 2,
			Response = 3
		};

		using BinaryTypesEnumType = UnderlyingType<BinaryTypes>;

		namespace BinarySubTypes {
			enum class Request : uint8_t {
				/*
				 * # SEE
				 * similar to get, can return NONE if there isn't.
				 * _see_ doesn't send ERROR
				 *
				 * # WANT
				 * this action is used when you want to see a different state in creature. For
				 * example
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
				NONE     = 3,
				SOIL     = 4
			};

			/*
			enum class Event : uint8_t {
				CREATURE_DEAD
			};
			*/

			static_assert(std::is_same_v<UnderlyingType<Request>, UnderlyingType<Error>>);
			static_assert(std::is_same_v<UnderlyingType<Error>, UnderlyingType<Response>>);

			using CodeSubTypes           = std::variant<Request, Error, Response>;
			using GenericResponseSubType = std::variant<Error, Response>;

		} // namespace BinarySubTypes

		using BinarySubTypesEnumType = UnderlyingType<BinarySubTypes::Error>;

		enum class BinaryNLT : uint8_t {
			FLOAT    = 1,
			DOUBLE   = 2,
			UINT8_T  = 3,
			UINT16_T = 4,
			UINT32_T = 5,
			UINT64_T = 6,
			INT      = 7,
		};

		using BinaryNLTEnumType = UnderlyingType<BinaryNLT>;

	} // namespace Types

} // namespace Karkinolution::Binary::Protocol
