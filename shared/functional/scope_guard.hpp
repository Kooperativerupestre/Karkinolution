#pragma once
#include <utility>

namespace KShared {
	namespace Functional {
		template <typename Function> class ScopeGuard {
			private:

				Function function;

			public:

				explicit ScopeGuard(Function function)
					: function{std::move(function)} {}

				ScopeGuard(const ScopeGuard &)            = delete;
				ScopeGuard &operator=(const ScopeGuard &) = delete;

				~ScopeGuard() noexcept {
					function();
				}
		};
	} // namespace Functional
} // namespace KShared