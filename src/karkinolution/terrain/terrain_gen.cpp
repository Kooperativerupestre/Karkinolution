#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/utils/k_random.hpp"

#include <FastNoise/FastNoise.h>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>
#include <karkinolution/terrain/terrain_gen.hpp>
#include <utility>

SoilTypes TerrainFactory::get_soil_type(const SignedNormalizedValue<float> &factor) {
	const NormalizedValue<float> normalized_factor = (factor.value() + 1.0f) / 2.0f;

	if (normalized_factor.value() < 0.25f) {
		return SoilTypes::ROCK;
	} else if (normalized_factor.value() < 0.5) {
		return SoilTypes::DIRT;
	} else if (normalized_factor.value() < 0.75) {
		return SoilTypes::SAND;
	} else {
		return SoilTypes::WATER;
	}
	std::unreachable();
}

Territory TerrainFactory::gen_terrain(const Size                  &size,
									  int                          seed,
									  float                        scale,
									  float                        epsilon,
									  const GeometryForms::Radius &min_radius,
									  const GeometryForms::Radius &max_radius) {
	std::vector<Vec3> coords;

	for (double lateral = 0.0; lateral <= size.lateral.value; lateral += epsilon) {
		for (double depth = 0.0; depth <= size.back.value; depth += epsilon) {
			for (double height = 0.0; height <= size.height.value; height += epsilon) {
				coords.push_back(Vec3{lateral, depth, height});
			}
		}
	}

	Territory territory{size};

	for (const auto &coord : coords) {
		const auto soil_type = TerrainFactory::get_soil_type(
			terrain_noise->GenSingle3D(coord.x, coord.y, coord.z, seed));

		const auto radius = RandomGenerators::generate(min_radius.value, max_radius.value);
		territory.add(SoilF::gen_soil_piece(soil_type, radius, coord));
	}
	return territory;
}
