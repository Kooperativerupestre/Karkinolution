#pragma once

#include "karkinolution/core/error.hpp"

namespace Karkinolution::Binary::Frames {
	class FrameError : public Errors::SimulationError {
		public:

			explicit FrameError(const std::string &message)
				: Errors::SimulationError(message) {}
	};

	class FrameInvariantError : public FrameError {
		public:

			explicit FrameInvariantError(const std::string &message)
				: FrameError(message) {}
	};
} // namespace Karkinolution::Binary::Frames
