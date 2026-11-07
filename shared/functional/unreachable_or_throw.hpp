#pragma once

#include "shared/functional/errors.hpp"

namespace KShared {
	namespace Functional {
		[[noreturn]] inline constexpr void unreachable_or_throw() {
			throw UnreachableError("Unreachable");
		}

		[[noreturn]] inline constexpr void unreachable_or_throw(const std::string &message) {
			throw UnreachableError(message);
		}
	} // namespace Functional
} // namespace KShared