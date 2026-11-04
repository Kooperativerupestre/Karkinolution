#pragma once

#include <karkinolution/organism/entities/creature/actions/presets.hpp>
#include <karkinolution/organism/entities/creature/metabolism/motor.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Metabolism::Resolver {
	namespace MetabolismResolver {
		std::variant<std::reference_wrapper<Karkinolution::Organisms::RawMeat>,
					 std::reference_wrapper<Karkinolution::Organisms::Nature::Grass::GrassMatter>>
		resolve_preset(const Actions::EatPreset &preset, Karkinolution::World::World &world);
	} // namespace MetabolismResolver
} // namespace Karkinolution::Organisms::Entities::Creature::Metabolism::Resolver
