#pragma once

#include "shared/core/error.hpp"

namespace KShared {
	namespace Functional {

		class FunctionalError : public Errors::SimulationError {
			public:

				explicit FunctionalError(const std::string &message)
					: SimulationError(message) {}
		};

		class UnreachableError : public FunctionalError {
			public:

				explicit UnreachableError(const std::string &message)
					: FunctionalError(message) {}
		};
	} // namespace Functional
} // namespace KShared