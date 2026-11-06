#include "karkinolution/organism/entities/corpse/corpse.hpp"
#include "karkinolution/organism/foods/foods.hpp"
#include "karkinolution/organism/values.hpp"

#include <karkinolution/core/id.hpp>
#include <karkinolution/organism/entities/corpse/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>

namespace Karkinolution::Organisms::Entities::Corpse::Physiology {

	Corpse generate_corpse(const Karkinolution::Organisms::Entities::Creature::Creature &creature) {
		RawMeat raw_meat{0};

		const auto &metabolism = creature.body.metabolism;
		const auto &morphology = creature.body.morphology;

		raw_meat.value += metabolism.energy.value() * 0.5f;
		raw_meat.value += metabolism.reserved.value() * 0.75f;
		raw_meat.value += morphology.area().value * 0.20f * metabolism.reserved.value();
		return Corpse{creature.body.morphology.size,
					  creature.position,
					  raw_meat,
					  Karkinolution::Core::Id::gen_id()};
	}

	Corpse generate_corpse(const Karkinolution::Organisms::Entities::Embryo::Embryo &embryo) {
		RawMeat raw_meat{0};

		raw_meat.value += embryo.energy.value() * 0.4f;
		raw_meat.value += embryo.area.value * 0.15f * embryo.energy.value();
		return Corpse{embryo.size(), embryo.position, raw_meat, Karkinolution::Core::Id::gen_id()};
	}

} // namespace Karkinolution::Organisms::Entities::Corpse::Physiology
