#pragma once

#include "karkinolution/core/id.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace API {
		namespace Corpse {
			const ::Corpse* see_corpse(const World &world, BaseIdType id);
		} // namespace Corpse
	} // namespace API
} // namespace Karkinolution