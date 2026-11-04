

#include "karkinolution/world/world.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/corpse/motor.hpp>
#include <karkinolution/organism/registry.hpp>

namespace Karkinolution::Organisms::Entities::Corpse::Motor {

	void run(Karkinolution::Organisms::Entities::Corpse::Corpse &corpse,
			 Karkinolution::World::World                        &world) {
		corpse.raw_meat.value *= 0.1;

		if (corpse.ready_to_disapear()) {
			world.remove(corpse.build_id());
		}
	}

} // namespace Karkinolution::Organisms::Entities::Corpse::Motor
