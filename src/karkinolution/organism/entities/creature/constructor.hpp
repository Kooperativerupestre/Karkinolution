#pragma once

#include "karkinolution/core/id.hpp"

#include <cstdint>
#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/internal_alias.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <optional>
#include <string>

namespace Karkinolution::Organisms::Entities::Creature::Constructor {
	// All fields are optional: when absent they are randomly chosen or derived
	// from the species canonical genome (global_creature_genomes).
	struct CreatureBlueprint {
			std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie =
				std::nullopt; // random if absent
			std::optional<Karkinolution::Core::Id::BaseIdType> id =
				std::nullopt;                               // random if absent
			std::optional<std::string> name = std::nullopt; // random if absent
			std::optional<Karkinolution::Organisms::Entities::Creature::Gender> gender =
				std::nullopt; // random if absent
			std::optional<Karkinolution::Math::Vec::Vec3> position =
				std::nullopt; // zero-vector if absent
	};

	namespace CreatureConstructor {

		[[nodiscard]] Karkinolution::Organisms::Entities::Creature::Creature
		from_blueprint(const CreatureBlueprint &blueprint = {});

	} // namespace CreatureConstructor
} // namespace Karkinolution::Organisms::Entities::Creature::Constructor
