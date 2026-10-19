#pragma once

#include "godot/extension/model/aliases.hpp"

#include <karkinolution/binary/deserialization/interpreters/corpse/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/territory/soil/alias.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>

namespace KarkinolutionGodot {
	namespace Translator {
		godot::Vector3            translate(const Vec3 &vec);
		GodotSize                 translate(const Size &size);
		godot::Ref<GodotCorpse>   translate(const CorpseDSI::Models::Corpse &corpse);
		godot::Ref<GodotCreature> translate(const CreatureDSI::Models::Creature &creature);
		GodotDamage               translate(const SoilDSI::Models::DamageComponent &damage);
		GodotMovementCost translate(const SoilDSI::Models::MovementCostComponent &movement_cost);
		godot::Ref<GodotSoil> translate(const SoilDSI::Models::Soil &soil);
		GodotProperty         translate(const GenericProperty &property);

	} // namespace Translator
} // namespace KarkinolutionGodot