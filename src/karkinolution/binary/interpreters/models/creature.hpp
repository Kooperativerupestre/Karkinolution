#pragma once

#include "karkinolution/core/id.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <string>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace Models {
				namespace Creature {
					struct Creature {
							Karkinolution::Organisms::Entities::Creature::Gender gender;
							Karkinolution::Organisms::Entities::Creature::CreatureSpecies specie;
							Karkinolution::Math::Vec::Vec3            position;
							std::string     name;
							Karkinolution::Core::Id::BaseIdType      id;
					};
				} // namespace Creature
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
