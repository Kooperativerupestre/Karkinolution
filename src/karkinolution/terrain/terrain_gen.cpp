#include "karkinolution/terrain/terrain_gen.hpp"

#include "karkinolution/utils/k_random.hpp"

#include <FastNoise/FastNoise.h>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>
#include <utility>

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
	auto simplex = FastNoise::New<FastNoise::Simplex>();
	simplex->SetScale(scale);

	auto terrain_noise = FastNoise::New<FastNoise::FractalFBm>();
	terrain_noise->SetSource(simplex);
	terrain_noise->SetOctaveCount(5);
	terrain_noise->SetGain(0.5f);
	terrain_noise->SetLacunarity(2.0f);

	Territory territory{size};

	for (double lateral = 0.0; lateral <= size.lateral.value; lateral += epsilon) {
		for (double depth = 0.0; depth <= size.back.value; depth += epsilon) {
			for (double height = 0.0; height <= size.height.value; height += epsilon) {

				const Vec3 coord{lateral, depth, height};

				const auto noise_value = terrain_noise->GenSingle3D(static_cast<float>(coord.x),
																	static_cast<float>(coord.y),
																	static_cast<float>(coord.z),
																	seed);

				const auto soil_type = TerrainFactory::get_soil_type(noise_value);

				const auto radius = RandomGenerators::generate(min_radius.value, max_radius.value);

				auto soil = SoilF::gen_soil_piece(soil_type, radius, coord);

				territory.add(soil.id, std::move(soil));
			}
		}
	}

	return territory;
}