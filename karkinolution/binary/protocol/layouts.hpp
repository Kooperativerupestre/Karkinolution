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
			using Resource = Next<Id, sizeof(Aliases::Request::FrameResource)>;

			using Payload = Field<Resource::END,
								  0>; // use it for whatever operation of request different from see

			namespace See {
				using Target  = Next<Resource, sizeof(Aliases::Request::FrameSeeTarget)>;
				using Payload = Field<Target::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace See

			namespace Create {
				using Payload = Field<Resource::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace Create

			namespace Cycle {
				using Payload = Field<Resource::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace Cycle

			namespace Erase {
				using Payload = Field<Resource::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace Erase

			namespace Want {
				using Payload = Field<Resource::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace Want
		} // namespace Request

		namespace Error {
			using RequestId = Next<SubType, sizeof(Aliases::RequestId)>;
			using Payload   = Field<RequestId::END, 0>;
		} // namespace Error

		namespace Response {
			using RequestId   = Next<SubType, sizeof(Aliases::RequestId)>;
			using Cardinality = Next<RequestId, sizeof(Aliases::Response::Cardinality)>;
			using Payload     = Field<Cardinality::END, 0>;

		} // namespace Response
	} // namespace Layout
} // namespace Karkinolution::Binary::Protocol
