#pragma once

#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "shared/binary/interpreters/models/stats.hpp"
#include "shared/core/id.hpp"
#include "shared/math/physic/vec/model.hpp"
#include "shared/math/units.hpp"

#include <string>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace Models {
				namespace Creature {
					struct Creature {
							Karkinolution::Organisms::Entities::Creature::Gender           gender;
							Karkinolution::Organisms::Entities::Creature::CreatureSpecies  specie;
							KShared::Math::Vec::Vec2                                       position;
							std::string                                                    name;
							KShared::Core::Id::BaseIdType                                  id;
							KShared::Math::Units::Size                                     size;
							KShared::Binary::Interpreters::Models::Stats::StatValue<float> life;
							KShared::Binary::Interpreters::Models::Stats::StatValue<float> energy;
							float                                                          age;
					};
				} // namespace Creature
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
