#pragma once
#include "godot/extension/model/creature.hpp"
#include "godot/extension/model/math/units.hpp"
#include "godot/extension/model/properties.hpp"
#include "godot/extension/model/terrain/soil.hpp"

#include <godot/extension/model/corpse.hpp>

using GodotCorpse       = KarkinolutionGodot::Models::Organisms::Corpse::GodotCorpse;
using GodotCreature     = KarkinolutionGodot::Models::Organisms::Creature::GodotCreature;
using GodotSoil         = KarkinolutionGodot::Models::Soils::GodotSoilPiece;
using GodotSize         = KarkinolutionGodot::Models::Math::Units::GodotSize;
using GodotProperty     = KarkinolutionGodot::Models::Properties::GodotProperty;
using GodotDamage       = KarkinolutionGodot::Models::Soils::Components::GodotDamage;
using GodotMovementCost = KarkinolutionGodot::Models::Soils::Components::GodotMovementCost;