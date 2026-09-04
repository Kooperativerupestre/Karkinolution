#pragma once

#include "karkinolution/math/stats/compile_values.hpp"

#include <FastNoise/FastNoise.h>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>

struct TerrainScale {
		static constexpr float NORMAL     = 0.1f;
		static constexpr float SMALL      = 0.01f;
		static constexpr float FRAGMENTED = 0.001f;
};

inline constexpr TerrainScale GLOBAL_SCALE{};

namespace TerrainFactory {

	SoilTypes get_soil_type(const SignedNormalizedValue<float> &factor);

	Territory gen_terrain(const Size                  &size,
						  int                          seed,
						  float                        scale,
						  float                        epsilon,
						  const GeometryForms::Radius &min_radius,
						  const GeometryForms::Radius &max_radius);

} // namespace TerrainFactory