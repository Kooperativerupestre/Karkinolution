#pragma once
#include "karkinolution/world/world.hpp"

#include <karkinolution/networking/frame/world/aliases.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Processor {
				ResponseFrame process(const RequestFrame &frame, Karkinolution::World::World &world);
			} // namespace Processor

		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
