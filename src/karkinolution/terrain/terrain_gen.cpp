#include "karkinolution/terrain/terrain_gen.hpp"

#include "karkinolution/utils/k_random.hpp"

#include <FastNoise/FastNoise.h>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>
#include <utility>
#include <vector>

SoilTypes TerrainFactory::get_soil_type(const SignedNormalizedValue<float> &factor) {

	const NormalizedValue<float> normalized_factor = (factor.value() + 1.0f) / 2.0f;

	if (normalized_factor.value() < 0.25f) {
		return SoilTypes::ROCK;
	}

	if (normalized_factor.value() < 0.5f) {
		return SoilTypes::DIRT;
	}

	if (normalized_factor.value() < 0.75f) {
		return SoilTypes::SAND;
	}

	return SoilTypes::WATER;
}

Territory TerrainFactory::gen_terrain(const Size                  &size,
									  int                          seed,
									  float                        scale,
									  float                        epsilon,
									  const GeometryForms::Radius &min_radius,
									  const GeometryForms::Radius &max_radius) {

	/*
	 * The noise graph belongs to this terrain generation operation.
	 *
	 * In particular, the FractalFBm generator must have its source
	 * configured before GenSingle3D() is called.
	 */
	auto simplex = FastNoise::New<FastNoise::Simplex>();

	simplex->SetScale(10.0f);

	auto terrain_noise = FastNoise::New<FastNoise::FractalFBm>();

	terrain_noise->SetSource(simplex);
	terrain_noise->SetOctaveCount(5);
	terrain_noise->SetGain(0.5f);
	terrain_noise->SetLacunarity(2.0f);

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

		const auto noise_value = terrain_noise->GenSingle3D(static_cast<float>(coord.x),
															static_cast<float>(coord.y),
															static_cast<float>(coord.z),
															seed);

		const auto soil_type = TerrainFactory::get_soil_type(noise_value);

		const auto radius = RandomGenerators::generate(min_radius.value, max_radius.value);

		territory.add(SoilF::gen_soil_piece(soil_type, radius, coord));
	}

	return territory;
}