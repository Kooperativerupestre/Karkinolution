#pragma once

#include <cstdint>

namespace KShared {
	namespace Functional {
		enum class UpdateOutput : uint8_t {
			DOES_NOT_EXIST,
			UPDATED
		};
	} // namespace Functional
} // namespace KShared