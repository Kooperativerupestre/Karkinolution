#pragma once

#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/core/id.hpp"

#include <cstdint>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace CoreBytes {
				namespace Identity {
					static_assert(sizeof(BaseIdType) == sizeof(int64_t));
					using Id = Serializer::Core::EightBytes;
				} // namespace Identity
			} // namespace CoreBytes
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

using IdBytes = Karkinolution::Binary::Interpreters::CoreBytes::Identity::Id;
