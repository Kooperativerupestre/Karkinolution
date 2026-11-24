#pragma once

#include "karkinolution/networking/frame/world/interpreters/models/corpse.hpp"
#include "karkinolution/networking/frame/world/interpreters/models/creature.hpp"
#include "karkinolution/networking/frame/world/interpreters/models/soil.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/terrain/soil/soil.hpp"
#include "shared/core/id.hpp"

namespace Karkinolution::Networking::ClientSide::World {
	using Creature        = Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::Creature;
	using Corpse          = Karkinolution::Networking::Frames::World::Interpreters::Models::Corpse::Corpse;
	using Soil            = Karkinolution::Networking::Frames::World::Interpreters::Models::Soil::Soil;
	using OrganismId      = KShared::Core::Id::BaseIdType;
	using SoilId          = Karkinolution::Terrain::Soil::SoilPieceId;
	using EntityType      = Karkinolution::Organisms::Entities::EntityTypes;
	using Gender          = Karkinolution::Organisms::Entities::Creature::Gender;
	using CreatureSpecies = Karkinolution::Organisms::Entities::Creature::CreatureSpecies;
} // namespace Karkinolution::Networking::ClientSide::World
