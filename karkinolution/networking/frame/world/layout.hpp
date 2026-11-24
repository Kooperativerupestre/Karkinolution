#pragma once

#include "karkinolution/networking/frame/base/layout.hpp"
#include "karkinolution/networking/frame/world/contract.hpp"
#include "shared/binary/fields.hpp"

#include <cstddef>
#include <cstdint>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Layout {
				using DeclaredSize = Base::Layout::DeclaredSize;
				using Type         = Base::Layout::Type;
				using SubType      = Base::Layout::SubType;

				inline constexpr std::size_t TOTAL_HEADER_SIZE = SubType::END;

				namespace Request {
					using Id = KShared::Binary::Next<SubType, sizeof(Frames::World::Request::Id)>;
					using Resource =
						KShared::Binary::Next<Id, sizeof(Detail::UnderlyingType<Frames::World::Request::Resource>)>;

					using GenericPayload                           = KShared::Binary::Field<Resource::END, 0>;
					inline constexpr std::size_t FIXED_HEADER_SIZE = GenericPayload::END;

					namespace ST {
						namespace See {
							using Target = KShared::Binary::
								Next<Resource, sizeof(Detail::UnderlyingType<Frames::World::Request::SeeTarget>)>;
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
					} // namespace ST
				} // namespace Request

				namespace Error {
					using RequestId      = KShared::Binary::Next<SubType, sizeof(Frames::World::Request::Id)>;
					using GenericPayload = KShared::Binary::Field<RequestId::END, 0>;

					inline constexpr std::size_t FIXED_HEADER_SIZE = GenericPayload::END;
				} // namespace Error

				namespace Response {
					using RequestId = KShared::Binary::Next<SubType, sizeof(Frames::World::Request::Id)>;
					using Cardinality =
						KShared::Binary::Next<RequestId,
											  sizeof(Detail::UnderlyingType<Frames::World::Response::Cardinality>)>;
					using GenericPayload = KShared::Binary::Field<Cardinality::END, 0>;

					inline constexpr std::size_t FIXED_HEADER_SIZE = GenericPayload::END;
				} // namespace Response
			} // namespace Layout
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
