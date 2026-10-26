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
							Gender          gender;
							CreatureSpecies specie;
							Vec3            position;
							std::string     name;
							BaseIdType      id;
					};
				} // namespace Creature
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
