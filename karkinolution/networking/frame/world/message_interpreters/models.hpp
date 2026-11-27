#pragma once

#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "shared/math/physic/vec/model.hpp"

#include <optional>
#include <string>

namespace Karkinolution::Networking::Frames::World::MessageInterpreters::Models::Creature {
	// CREATE requests omit domain-only identity fields and carry only client-selectable values.
	struct CreaturePreset {
			std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender;
			std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie;
			std::optional<KShared::Math::Vec::Vec2>                                      position;
			std::optional<std::string>                                                   name;
	};
} // namespace Karkinolution::Networking::Frames::World::MessageInterpreters::Models::Creature
