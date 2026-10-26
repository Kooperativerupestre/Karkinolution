#pragma once
#include "karkinolution/world/world.hpp"

#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/binary/frames/parser.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Processor {
				ResponseFrame process(const RequestFrame &frame, World &world);
			} // namespace Processor

		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution
