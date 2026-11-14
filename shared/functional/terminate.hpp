#pragma once

#include <string>

namespace KShared {
	namespace Functional {
		/*
		terminate() is simply a function that behaves like std::terminate(),
		but also allows you to add logging before terminating.

		The version without a message allows programmers to use the function
		without providing a message initially, while retaining the ability to
		add one later without having to modify much of the existing code.
		*/
		[[noreturn]] void terminate() noexcept;
		[[noreturn]] void terminate(const std::string &message) noexcept;
	} // namespace Functional
} // namespace KShared