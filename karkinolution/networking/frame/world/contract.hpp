#pragma once

#include <cstdint>
#include <shared/binary/numeric_type.hpp>
#include <type_traits>
#include <variant>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			using DeclaredSize = std::uint32_t;

			enum class Type : uint8_t {
				Request  = 1,
				Error    = 2,
				Response = 3
			};

			namespace Request {
				using Id = uint64_t;

				enum class Resource : uint8_t {
					CREATURE = 1,
					EMBRYO   = 2,
					SOIL     = 3,
					CORPSE   = 4,
					EGG      = 5,
					// A world has one territory, so SEE TERRITORY only supports target ONE.
					TERRITORY = 6
				};

				enum class SubType : uint8_t {
					SEE    = 1,
					WANT   = 2,
					ERASE  = 3,
					CREATE = 4,
					CYCLE  = 5
				};

				enum class SeeTarget : uint8_t {
					ONE = 1,
					ALL = 2
				};

				namespace ST {
					namespace See {}

					namespace Want {}

					namespace Erase {}

					namespace Create {}

					namespace Cycle {}
				} // namespace ST
			} // namespace Request

			namespace Error {
				using RequestId = Request::Id;

				enum class SubType : uint8_t {
					CREATURE_WAS_NOT_FOUND = 1,
					CORPSE_WAS_NOT_FOUND   = 2
				};

				namespace ST {}
			} // namespace Error

			namespace Response {
				using RequestId = Request::Id;

				enum class SubType : uint8_t {
					CREATURE = 0,
					CORPSE   = 1,
					NONE     = 3,
					SOIL     = 4,
					// A world has one territory, so TERRITORY responses only support cardinality ONE.
					TERRITORY = 5,
					BOOLEAN   = 6
				};

				enum class Cardinality : uint8_t {
					ONE = 1,
					ALL = 2
				};
			} // namespace Response

			using NumericType = KShared::Binary::NumericType;

			namespace Detail {
				template <typename T> using UnderlyingType = std::underlying_type_t<T>;
				using SubTypeVariant  = std::variant<Request::SubType, Error::SubType, Response::SubType>;
				using ResponseVariant = std::variant<Error::SubType, Response::SubType>;
			} // namespace Detail

		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
