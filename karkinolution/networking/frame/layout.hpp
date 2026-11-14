#pragma once

#include "karkinolution/binary/protocol/aliases.hpp"
#include "karkinolution/networking/frame/contract.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/binary/fields.hpp"

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Layout {
				using DeclaredSize = KShared::Binary::Field<0, sizeof(DeclaredSize)>;
				using Type =
					KShared::Binary::Next<DeclaredSize, sizeof(Binary::Protocol::Aliases::UnderlyingType<Type>)>;
				using SubType = KShared::Binary::Next<Type, sizeof(uint8_t)>;

				namespace SystemCall {
					using Id             = KShared::Binary::Next<SubType, sizeof(Frames::SystemCall::Id)>;
					using GenericPayload = KShared::Binary::Field<Id::END, 0>;
					inline constexpr std::size_t FIXED_HEADER_SIZE = GenericPayload::END;

					namespace ST {
						namespace Config {
							using Payload = KShared::Binary::Field<Id::END, 0>;

							inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
						} // namespace Config

						namespace GetAllWorldIds {
							using Payload = KShared::Binary::Field<Id::END, 0>;

							inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
						} // namespace GetAllWorldIds
					} // namespace ST
				} // namespace SystemCall

				namespace SystemOutput {
					using CallId         = KShared::Binary::Next<SubType, sizeof(Frames::SystemCall::Id)>;
					using GenericPayload = KShared::Binary::Field<CallId::END, 0>;
					inline constexpr std::size_t FIXED_HEADER_SIZE = GenericPayload::END;

					namespace ST {
						namespace WorldState {
							using Payload = KShared::Binary::Field<CallId::END, 0>;

							inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
						} // namespace WorldState

						namespace AllWorldIds {
							using Payload = KShared::Binary::Field<CallId::END, 0>;

							inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
						} // namespace AllWorldIds
					} // namespace ST
				} // namespace SystemOutput
			} // namespace Layout
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution
