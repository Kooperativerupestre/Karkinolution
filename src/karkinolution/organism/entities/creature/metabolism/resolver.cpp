#include "karkinolution/organism/foods/foods.hpp"

#include <iostream>
#include <karkinolution/organism/entities/creature/metabolism/resolver.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Metabolism::Resolver {
	std::variant<
		std::reference_wrapper<Karkinolution::Organisms::RawMeat>,
		std::reference_wrapper<Karkinolution::Organisms::Nature::Grass::GrassMatter>>
	MetabolismResolver::resolve_preset(const Actions::EatPreset &preset,
									   Karkinolution::World::World &world) {
		if (preset.hint == FoodHint::GRASS_MATTER) {
			std::cout << "Grass does not exist currently" << "\n";
		}

		auto &corpse = std::get<Karkinolution::Organisms::Entities::Corpse::Corpse>(
			world.at_entity(std::get<EntityId>(preset.id)));
		return corpse.raw_meat;
	}
} // namespace Karkinolution::Organisms::Entities::Creature::Metabolism::Resolver
