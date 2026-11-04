#pragma once
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/terrain/entity_map.hpp"
#include "karkinolution/terrain/terrain.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Physic {
				namespace Analyzer {
					enum class CanBeOutput : uint8_t {
						OVERLAP     = 1,
						OUTSIDE_MAP = 2,
						OK          = 3
					};

					CanBeOutput can_be(const Entity                      &entity,
									   const Karkinolution::World::World &world);
				} // namespace Analyzer
			} // namespace Physic
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution

namespace PhysicAnalyzer = Karkinolution::Organisms::Entities::Physic::Analyzer;