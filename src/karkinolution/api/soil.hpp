#pragma once

#include "karkinolution/core/id.hpp"

#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace API {
		namespace Soil {
			const SoilPiece* see_soil(const World &world, BaseIdType id);
		} // namespace Soil
	} // namespace API
} // namespace Karkinolution