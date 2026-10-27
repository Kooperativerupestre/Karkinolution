#pragma once

#include "karkinolution/core/error.hpp"

namespace Karkinolution {
	namespace Functional {
		namespace Errors {
			class FunctionalError : public SimulationError {
				public:

					explicit FunctionalError(const std::string &message)
						: SimulationError(message) {}
			};

			class UnreachableError : public FunctionalError {
				public:

					explicit UnreachableError(const std::string &message)
						: FunctionalError(message) {}
			};
		} // namespace Errors
	} // namespace Functional
} // namespace Karkinolution