#pragma once

#include "shared/binary/serializer.hpp"
#include "shared/core/id.hpp"

#include <cstdint>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace CoreBytes {
				namespace Identity {
					static_assert(sizeof(KShared::Core::Id::BaseIdType) == sizeof(int64_t));
					using Id = KShared::Binary::Serializer::Core::EightBytes;
				} // namespace Identity
			} // namespace CoreBytes
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

using IdBytes = Karkinolution::Binary::Interpreters::CoreBytes::Identity::Id;
