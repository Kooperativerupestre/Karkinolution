#pragma once
#include <cstdint>
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

namespace Frames {

	namespace Types {
		template <typename T> using UnderlyingType = std::underlying_type_t<T>;

		using RequestId = uint64_t;

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
				 * similar to get, can return nothing if there isn't
				 *
				 * # WANT
				 * this action is used when you want to see a different state in creature. For
				 example
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


			/*
			enum class Event : uint8_t {
				CREATURE_DEAD
			};*/

			static_assert(std::is_same_v<UnderlyingType<Request>, UnderlyingType<Error>>);

			static_assert(std::is_same_v<UnderlyingType<Error>, UnderlyingType<Response>>);

			using CodeSubTypes           = std::variant<Request, Error, Response>;
			using GenericResponseSubType = std::variant<Error, Response>;


		} // namespace BinarySubTypes

		using BinarySubTypesEnumType = UnderlyingType<BinarySubTypes::Error>;
		using FrameDeclaredSize      = uint32_t;
		using FrameType              = UnderlyingType<BinaryTypes>;
		using FrameSubType           = UnderlyingType<BinarySubTypes::Request>;
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
		using BinaryNLTEnumType = UnderlyingType<BinaryNLT>;


	} // namespace Types

	namespace FieldsBytes {
		inline constexpr std::size_t SUB_TYPE      = sizeof(Types::FrameSubType);      // bytes
		inline constexpr std::size_t TYPE          = sizeof(Types::FrameType);         // bytes
		inline constexpr std::size_t DECLARED_SIZE = sizeof(Types::FrameDeclaredSize); // bytes
		inline constexpr std::size_t MESSAGE_HEADER_BYTES = DECLARED_SIZE + TYPE + SUB_TYPE;

		namespace RequestTypes {
			namespace Request {
				inline constexpr std::size_t RESOURCE = sizeof(Types::RequestResource);
				inline constexpr std::size_t ID       = sizeof(Types::RequestId);
			} // namespace Request
		} // namespace RequestTypes

	} // namespace FieldsBytes

	namespace Offsets {
		inline constexpr std::size_t TO_GET_SIZE = 0;

		inline constexpr std::size_t TO_GET_TYPE     = TO_GET_SIZE + FieldsBytes::DECLARED_SIZE;
		inline constexpr std::size_t TO_GET_SUB_TYPE = TO_GET_TYPE + FieldsBytes::TYPE;

		namespace RequestTypes {
			namespace Request {
				inline constexpr std::size_t TO_GET_ID = TO_GET_SUB_TYPE + FieldsBytes::SUB_TYPE;

				inline constexpr std::size_t TO_GET_RESOURCE =
					TO_GET_ID + FieldsBytes::RequestTypes::Request::ID;

				inline constexpr std::size_t TO_GET_PAYLOAD =
					TO_GET_RESOURCE + FieldsBytes::RequestTypes::Request::RESOURCE;
			} // namespace Request
		} // namespace RequestTypes

		namespace ResponseTypes {

			inline constexpr std::size_t TO_GET_REQUEST_ID =
				TO_GET_SUB_TYPE + FieldsBytes::SUB_TYPE;
			inline constexpr std::size_t TO_GET_PAYLOAD =
				TO_GET_REQUEST_ID + FieldsBytes::RequestTypes::Request::ID;
		} // namespace ResponseTypes

	} // namespace Offsets
} // namespace Frames

static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

static_assert(sizeof(int) == 4);
static_assert(sizeof(long long) == 8);
