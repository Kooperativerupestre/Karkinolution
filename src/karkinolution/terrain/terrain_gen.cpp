#include "karkinolution/terrain/terrain_gen.hpp"

#include "karkinolution/functional/k_random.hpp"
#include "karkinolution/math/units.hpp"

#include <FastNoise/FastNoise.h>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>
#include <utility>

namespace Karkinolution::Terrain::TerrainFactory {

	Soil::SoilTypes get_soil_type(const Math::LV::CT::SignedNormalizedValue<float> &factor) {

		const NormalizedValue<float> normalized_factor = (factor.value() + 1.0f) / 2.0f;

		if (normalized_factor.value() < 0.25f) {
			return Soil::SoilTypes::ROCK;
		}

		if (normalized_factor.value() < 0.5f) {
			return Soil::SoilTypes::DIRT;
		}

		if (normalized_factor.value() < 0.75f) {
			return Soil::SoilTypes::SAND;
		}

		return Soil::SoilTypes::WATER;
	}

	// Soil height is temporarily fixed to simplify interface and physics development.
	// Variable heights will be reintroduced later.
	Territory gen_terrain(const Math::Units::Size      &size,
						  int                           seed,
						  float                         scale,
						  float                         epsilon,
						  const Math::Geometry::Radius &min_radius,
						  const Math::Geometry::Radius &max_radius) {
		auto simplex = FastNoise::New<FastNoise::Simplex>();
		simplex->SetScale(scale);

		auto terrain_noise = FastNoise::New<FastNoise::FractalFBm>();
		terrain_noise->SetSource(simplex);
		terrain_noise->SetOctaveCount(5);
		terrain_noise->SetGain(0.5f);
		terrain_noise->SetLacunarity(2.0f);

		Territory territory{size};

		for (double width = 0.0; width <= size.width.value; width += epsilon) {
			for (double height = 0.0; height <= size.height.value; height += epsilon) {

				const Vec2 coord{width, height};

				const auto noise_value =
					terrain_noise->GenSingle2D(static_cast<float>(coord.x), static_cast<float>(coord.y), seed);

				const auto soil_type = TerrainFactory::get_soil_type(noise_value);

				const auto radius = Functional::KRandom::RandomGenerators::generate(min_radius.value, max_radius.value);

				auto soil = Soil::SoilF::gen_soil_piece(soil_type, radius, coord);

				territory.add(soil.id, std::move(soil));
			}
		}

		return territory;
	}

} // namespace Karkinolution::Terrain::TerrainFactory
