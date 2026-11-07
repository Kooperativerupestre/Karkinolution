#pragma once

#include "shared/core/error.hpp"

namespace Karkinolution::Binary::Frames {
	class FrameError : public KShared::Errors::SimulationError {
		public:

			explicit FrameError(const std::string &message)
				: KShared::Errors::SimulationError(message) {}
	};

	class FrameInvariantError : public FrameError {
		public:

			explicit FrameInvariantError(const std::string &message)
				: FrameError(message) {}
	};
} // namespace Karkinolution::Binary::Frames
