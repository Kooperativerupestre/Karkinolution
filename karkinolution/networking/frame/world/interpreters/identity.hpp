#pragma once

#include "shared/binary/serializer.hpp"
#include "shared/core/id.hpp"

#include <cstdint>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World::Interpreters {
			namespace CoreBytes {
				namespace Identity {
					static_assert(sizeof(KShared::Core::Id::BaseIdType) == sizeof(int64_t));
					using Id = KShared::Binary::Serializer::Core::EightBytes;
				} // namespace Identity
			} // namespace CoreBytes
		} // namespace Frames::World::Interpreters
	} // namespace Networking
} // namespace Karkinolution

using IdBytes = Karkinolution::Networking::Frames::World::Interpreters::CoreBytes::Identity::Id;
