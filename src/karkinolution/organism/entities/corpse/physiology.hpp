#pragma once
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Corpse {
				namespace Physiology {
					Corpse generate_corpse(const Creature::Creature &creature);
					Corpse generate_corpse(const Embryo::Embryo &embryo);
				} // namespace Physiology
			} // namespace Corpse
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
