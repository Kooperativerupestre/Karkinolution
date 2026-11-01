#pragma once

#include "karkinolution/core/error.hpp"

namespace Karkinolution {
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
} // namespace Karkinolution