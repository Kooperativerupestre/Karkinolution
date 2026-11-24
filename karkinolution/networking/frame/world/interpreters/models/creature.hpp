#pragma once

#include "karkinolution/organism/entities/creature/brain/intents.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "shared/binary/interpreters/models/stats.hpp"
#include "shared/core/id.hpp"
#include "shared/math/physic/vec/model.hpp"
#include "shared/math/units.hpp"

#include <cstdint>
#include <string>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World::Interpreters {
			namespace Models {
				namespace Creature {
					using StatValue = KShared::Binary::Interpreters::Models::Stats::StatValue<float>;

					struct MorphologyData {
							KShared::Math::Units::Size size;
							KShared::Math::Units::Mass mass;
					};

					struct VitalData {
							StatValue life;
							StatValue health;
					};

					struct MetabolismData {
							StatValue energy;
							StatValue reserved_energy;
					};

					struct IntentData {
							std::uint32_t                                                             time;
							Karkinolution::Organisms::Entities::Creature::Brain::Intents::IntentTypes type;
					};

					struct BrainData {
							IntentData intent;
					};

					struct Creature {
							Karkinolution::Organisms::Entities::Creature::Gender          gender;
							Karkinolution::Organisms::Entities::Creature::CreatureSpecies specie;
							KShared::Math::Vec::Vec2                                      position;
							std::string                                                   name;
							KShared::Core::Id::BaseIdType                                 id;
							MorphologyData                                                morphology;
							VitalData                                                     vital;
							MetabolismData                                                metabolism;
							BrainData                                                     brain;
							float                                                         age;
					};
				} // namespace Creature
			} // namespace Models
		} // namespace Frames::World::Interpreters
	} // namespace Networking
} // namespace Karkinolution
