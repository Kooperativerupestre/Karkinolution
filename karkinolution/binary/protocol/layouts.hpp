#pragma once

#include "karkinolution/binary/protocol/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "shared/binary/fields.hpp"
#include "shared/core/id.hpp"

#include <cstddef>

namespace Karkinolution::Binary::Protocol {
	namespace Layout {
		using DeclaredSize = KShared::Binary::Field<0, sizeof(Aliases::FrameDeclaredSize)>;
		using Type         = KShared::Binary::Next<DeclaredSize, sizeof(Aliases::FrameType)>;
		using SubType      = KShared::Binary::Next<Type, sizeof(Aliases::FrameSubType)>;

		inline constexpr std::size_t TOTAL_HEADER_SIZE = SubType::END;

		// SL = Sub types layouts

		namespace Request {
			using Id       = KShared::Binary::Next<SubType, sizeof(Aliases::RequestId)>;
			using Resource = KShared::Binary::Next<Id, sizeof(Aliases::Request::FrameResource)>;

			using Payload = KShared::Binary::Field<Resource::END,
												   0>; // use it for whatever operation of request different from see

			namespace See {
				using Target  = KShared::Binary::Next<Resource, sizeof(Aliases::Request::FrameSeeTarget)>;
				using Payload = KShared::Binary::Field<Target::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace See

			namespace Create {
				using Payload = KShared::Binary::Field<Resource::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace Create

			namespace Cycle {
				using Payload = KShared::Binary::Field<Resource::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace Cycle

			namespace Erase {
				using Payload = KShared::Binary::Field<Resource::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace Erase

			namespace Want {
				using Payload = KShared::Binary::Field<Resource::END, 0>;

				inline constexpr std::size_t FIXED_HEADER_SIZE = Payload::END;
			} // namespace Want
		} // namespace Request

		namespace Error {
			using RequestId = KShared::Binary::Next<SubType, sizeof(Aliases::RequestId)>;
			using Payload   = KShared::Binary::Field<RequestId::END, 0>;
		} // namespace Error

		namespace Response {
			using RequestId   = KShared::Binary::Next<SubType, sizeof(Aliases::RequestId)>;
			using Cardinality = KShared::Binary::Next<RequestId, sizeof(Aliases::Response::Cardinality)>;
			using Payload     = KShared::Binary::Field<Cardinality::END, 0>;

		} // namespace Response
	} // namespace Layout
} // namespace Karkinolution::Binary::Protocol
