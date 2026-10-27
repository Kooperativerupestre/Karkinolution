#pragma once

#include "karkinolution/core/error.hpp"

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			class FrameError : public SimulationError {
				public:

					explicit FrameError(const std::string &message)
						: SimulationError(message) {}
			};
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution