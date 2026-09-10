#include "karkinolution/organism/foods/foods.hpp"

#include <iostream>
#include <karkinolution/organism/entities/creature/metabolism/resolver.hpp>
#include <karkinolution/world/world.hpp>

std::variant<std::reference_wrapper<RawMeat>, std::reference_wrapper<GrassMatter>>
MetabolismResolver::resolve_preset(const EatPreset &preset, World &world) {
	if (preset.hint == FoodHint::GRASS_MATTER) {
		std::cout << "Grass does not exist currently" << "\n";
	}

	return world.organism_registry.entities.at_corpse(std::get<EntityId>(preset.id)).raw_meat;
}
