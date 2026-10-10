#pragma once

#include "karkinolution/binary/serialization/serializer.hpp"
#include "karkinolution/core/id.hpp"

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
