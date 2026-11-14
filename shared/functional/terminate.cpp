#include <exception>
#include <iostream>
#include <shared/functional/terminate.hpp>

namespace KShared {
	[[noreturn]] void Functional::terminate() noexcept {
		std::terminate();
	}

	[[noreturn]] void Functional::terminate(const std::string &message) noexcept {
		std::cout << message;
		std::terminate();
	}
} // namespace KShared