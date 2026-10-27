#pragma once

#include "karkinolution/functional/errors.hpp"

namespace Karkinolution {
	namespace Functional {
		[[noreturn]] inline constexpr void unreachable_or_throw() {
			throw Errors::UnreachableError("Unreachable");
		}

		[[noreturn]] inline constexpr void unreachable_or_throw(const std::string &message) {
			throw Errors::UnreachableError(message);
		}
	} // namespace Functional
} // namespace Karkinolution