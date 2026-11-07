#pragma once

#include "shared/math/stats/compile_values.hpp"

#include <FastNoise/FastNoise.h>
#include <shared/math/geometry/models.hpp>
#include <shared/math/physic/vec/model.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>

namespace Karkinolution {
	namespace Terrain {
		namespace TerrainFactory {
			struct TerrainScale {
					static constexpr float NORMAL     = 0.1f;
					static constexpr float SMALL      = 0.01f;
					static constexpr float FRAGMENTED = 0.001f;
			};

			inline constexpr TerrainScale GLOBAL_SCALE{};

			Soil::SoilTypes get_soil_type(const KShared::Math::LV::CT::SignedNormalizedValue<float> &factor);

			Territory gen_terrain(const KShared::Math::Units::Size      &size,
								  int                           seed,
								  float                         scale,
								  float                         epsilon,
								  const KShared::Math::Geometry::Radius &min_radius,
								  const KShared::Math::Geometry::Radius &max_radius);


		} // namespace TerrainFactory
	} // namespace Terrain
} // namespace Karkinolution
