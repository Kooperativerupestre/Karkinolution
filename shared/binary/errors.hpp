#pragma once

#include "shared/core/error.hpp"

namespace KShared::Binary {
	class ByteError : public KShared::Errors::SimulationError {
		public:

			explicit ByteError(const std::string &message)
				: KShared::Errors::SimulationError(message) {}
	};
} // namespace KShared::Binary
