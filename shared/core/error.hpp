#pragma once
#include <stdexcept>
#include <string>

namespace KShared {
	namespace Errors {


		class SimulationError : public std::runtime_error {
			public:

				explicit SimulationError(const std::string &message)
					: std::runtime_error(message) {}
		};

	} // namespace Errors
} // namespace KShared
