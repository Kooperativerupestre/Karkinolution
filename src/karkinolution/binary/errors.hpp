#pragma once

#include "karkinolution/core/error.hpp"

namespace Karkinolution::Binary {
	class ByteError : public Errors::SimulationError {
		public:

			explicit ByteError(const std::string &message)
				: Errors::SimulationError(message) {}
	};
} // namespace Karkinolution::Binary
