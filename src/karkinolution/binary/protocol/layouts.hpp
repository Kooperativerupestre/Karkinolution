#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"

#include <cstddef>

namespace Karkinolution::Binary::Protocol {
	namespace Layout {
		using DeclaredSize = Field<0, sizeof(Aliases::FrameDeclaredSize)>;
		using Type         = Next<DeclaredSize, sizeof(Aliases::FrameType)>;
		using SubType      = Next<Type, sizeof(Aliases::FrameSubType)>;

		inline constexpr std::size_t TOTAL_HEADER_SIZE = SubType::END;

		namespace Request {
			using Id       = Next<SubType, sizeof(Aliases::RequestId)>;
			using Resource = Next<Id, sizeof(Aliases::FrameResource)>;
			using Payload  = Field<Resource::END, 0>;
		} // namespace Request

		namespace Response {
			using RequestId = Next<SubType, sizeof(Aliases::RequestId)>;
			using Payload   = Field<RequestId::END, 0>;
		} // namespace Response
	} // namespace Layout
} // namespace Karkinolution::Binary::Protocol