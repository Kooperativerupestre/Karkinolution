#pragma once

#include "karkinolution/world/world.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Corpse {

				namespace Motor {
					void run(Corpse &corpse, Karkinolution::World::World &world);
				} // namespace Motor
			} // namespace Corpse
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
