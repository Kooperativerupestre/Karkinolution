#pragma once

#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/nature/natures.hpp>

namespace Karkinolution {
	namespace Organisms {
		struct OrganismRegistry {
				Entities::EntitiesRegistry entities_registry;
				Nature::NatureRegistry     natures_registry;
		};
	} // namespace Organisms
} // namespace Karkinolution
