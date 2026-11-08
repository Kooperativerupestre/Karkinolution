#pragma once
#include "karkinolution/binary/interpreters/models/soil.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/terrain/soil/soil.hpp"
#include "shared/core/id.hpp"

#include <karkinolution/binary/interpreters/models/corpse.hpp>
#include <karkinolution/binary/interpreters/models/creature.hpp>

namespace KarkinolutionSDK {
	namespace World {
		using Creature        = Karkinolution::Binary::Interpreters::Models::Creature::Creature;
		using Corpse          = Karkinolution::Binary::Interpreters::Models::Corpse::Corpse;
		using Soil            = Karkinolution::Binary::Interpreters::Models::Soil::Soil;
		using OrganismId      = KShared::Core::Id::BaseIdType;
		using SoilId          = Karkinolution::Terrain::Soil::SoilPieceId;
		using EntityType      = Karkinolution::Organisms::Entities::EntityTypes;
		using Gender          = Karkinolution::Organisms::Entities::Creature::Gender;
		using CreatureSpecies = Karkinolution::Organisms::Entities::Creature::CreatureSpecies;
	} // namespace World
} // namespace KarkinolutionSDK