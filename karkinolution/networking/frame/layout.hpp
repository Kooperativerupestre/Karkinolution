#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"
#include "karkinolution/networking/frame/contract.hpp"
#include "karkinolution/world/world.hpp"

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Layout {
				using DeclaredSize = Binary::Field<0, sizeof(DeclaredSize)>;
				using Type    = Binary::Next<DeclaredSize, sizeof(Binary::Protocol::Aliases::UnderlyingType<Type>)>;
				using SubType = Binary::Next<Type, sizeof(uint8_t)>;

				namespace SystemCall {
					using Id             = Binary::Next<SubType, sizeof(Frames::SystemCall::Id)>;
					using GenericPayload = Binary::Field<Id::END, 0>;
					inline constexpr std::size_t FIXED_HEADER_SIZE = GenericPayload::END;

					namespace ST {
						namespace Config {
							using Payload = Binary::Field<Id::END, 0>;

							inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
						} // namespace Config

						namespace GetAllWorldIds {
							using Payload = Binary::Field<Id::END, 0>;

							inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
						} // namespace GetAllWorldIds
					} // namespace ST
				} // namespace SystemCall

				namespace SystemOutput {
					using CallId         = Binary::Next<SubType, sizeof(Frames::SystemCall::Id)>;
					using GenericPayload = Binary::Field<CallId::END, 0>;
					inline constexpr std::size_t FIXED_HEADER_SIZE = GenericPayload::END;

					namespace ST {
						namespace WorldState {
							using Payload = Binary::Field<CallId::END, 0>;

							inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
						} // namespace WorldState

						namespace AllWorldIds {
							using Payload = Binary::Field<CallId::END, 0>;

							inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
						} // namespace AllWorldIds
					} // namespace ST
				} // namespace SystemOutput
			} // namespace Layout
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution
