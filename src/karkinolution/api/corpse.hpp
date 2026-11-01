#pragma once

#include "karkinolution/core/id.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace API {
		namespace Corpse {
			const Karkinolution::Organisms::Entities::Corpse::Corpse*
			see_corpse(const Karkinolution::World::World &world,
					   Karkinolution::Core::Id::BaseIdType id);
		} // namespace Corpse
	} // namespace API
} // namespace Karkinolution