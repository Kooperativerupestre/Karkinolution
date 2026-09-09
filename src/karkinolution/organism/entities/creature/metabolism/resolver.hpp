#pragma once

#include <karkinolution/organism/entities/creature/actions/presets.hpp>
#include <karkinolution/organism/entities/creature/metabolism/motor.hpp>
#include <karkinolution/world/world.hpp>

namespace MetabolismResolver {
	std::variant<std::reference_wrapper<RawMeat>, std::reference_wrapper<Grass>>
	resolve_preset(const EatPreset &preset, World &world);
} // namespace MetabolismResolver