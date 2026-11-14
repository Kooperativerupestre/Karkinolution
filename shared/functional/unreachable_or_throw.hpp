#pragma once

#include "shared/functional/errors.hpp"

namespace KShared {
	namespace Functional {
		/*
		The unreachable_or_throw function serves primarily as an indication that a particular
		part of the code is considered unreachable (or should be), but this is not guaranteed.
		If that code is reached, you want an explicit error instead of undefined behavior
		(which is the typical behavior of std::unreachable()).
		*/
		template <typename Error = UnreachableError> [[noreturn]] inline constexpr void unreachable_or_throw() {
			throw Error("Unreachable");
		}

		template <typename Error = UnreachableError>
		[[noreturn]] inline constexpr void unreachable_or_throw(const std::string &message) {
			throw Error(message);
		}
	} // namespace Functional
} // namespace KShared