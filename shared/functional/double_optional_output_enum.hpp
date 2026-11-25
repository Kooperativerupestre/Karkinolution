#pragma once

#include <cstdint>

namespace KShared {
	namespace Functional {
		// This class should be used to represent states of double non-existence of something.
		// For example, an unordered_map whose value is a std::optional. There are 3 states:
		// exists, does not exist, exists, but has no value.
		enum class DoubleOptional : uint8_t {
			EXISTS_AND_HAS,
			DOES_NOT_EXIST,
			EXISTS_AND_HAS_NOT
		};
	} // namespace Functional
} // namespace KShared