#pragma once

#include "shared/binary/fields.hpp"

#include <cstdint>
#include <shared/core/id.hpp>
#include <type_traits>
#include <variant>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::System {
			using DeclaredSize = std::uint32_t;
			// Declared size = size after the declared_size field


			enum class Type : uint8_t {
				SystemCall   = 2,
				SystemOutput = 1
			};

			/*
			A subtype that cannot have more fields than its parent type falls into the generic subtype
			category. A generic subtype has the advantage of not requiring its own parser implementation,
			reducing code duplication without breaking the contract. It is essentially a contract between
			the code and its layout.

			For each type, you will typically find the so-called generic parser functions in core.hpp.
			These functions will always start with the "generic" prefix. They are then used in parser.hpp
			(within that type's directory) instead of calling a dedicated implementation for that subtype.
			*/
			namespace SystemCall {
				using Id = KShared::Core::Id::BaseIdType;
				enum class SubType : uint8_t {
					CONFIG            = 0, // Yes
					GET_ALL_WORLD_IDS = 4, // No
					// This subtype is dedicated because AutoConnect extends the base header.
					CREATE_WORLD = 5 // Yes
				};

				namespace ST {
					namespace Config {}

					namespace GetAllWorldIds {}

					namespace CreateWorld {}
				} // namespace ST
			} // namespace SystemCall

			namespace SystemOutput {
				using CallId = KShared::Core::Id::BaseIdType;


				enum class SubType : uint8_t {
					// Can have more fields than SystemOutput base?
					AllWorldIds  = 1, // No
					WorldState   = 2, // Yes
					None         = 3, // No
					ConfigOutput = 4, // No
					WorldCreated = 5  // No
				};
			} // namespace SystemOutput

			namespace Detail {
				template <typename T> using UnderlyingType = std::underlying_type_t<T>;
				using SubTypeVariant                       = std::variant<SystemCall::SubType, SystemOutput::SubType>;
			} // namespace Detail
		} // namespace Frames::System
	} // namespace Networking
} // namespace Karkinolution
