#include "karkinolution/organism/foods/foods.hpp"

#include <iostream>
#include <karkinolution/organism/entities/creature/metabolism/resolver.hpp>
#include <karkinolution/world/world.hpp>

std::variant<std::reference_wrapper<RawMeat>, std::reference_wrapper<Grass>>
MetabolismResolver::resolve_preset(const EatPreset &preset, World &world) {
	if (preset.hint == FoodHint::GRASS) {
		std::cout << "Grass does not exist currently" << "\n";
	}

	return world.organism_registry.entities.at_corpse(std::get<Id>(preset.id)).raw_meat;
}
