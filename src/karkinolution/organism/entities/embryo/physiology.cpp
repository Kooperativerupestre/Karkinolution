#include "karkinolution/organism/values.hpp"

#include <algorithm>
#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/embryo/physiology.hpp>

namespace Karkinolution::Organisms::Entities::Embryo::Physiology {
	bool EmbryoPhysiology::is_dead(const Embryo &embryo) {
		return embryo.life.is_zero() || embryo.energy.is_zero();
	}

	float EmbryoPhysiology::basal_metabolism(const Embryo &embryo) {
		return EMBRYO_BASAL_METABOLISM;
	}

	NormalizedValue<float>
	EmbryoGrowingPhysiology::get_increment(const NormalizedValue<float> &ratio,
										   const NormalizedValue<float> &factor) {
		if (ratio.value() < Math::Approx<float>(0.5)) {
			float x = (0.5f - ratio.value()) / 0.5f;
			return 1.0f + x * x;
		}
		float x = (ratio.value() - 0.5f) / 0.5;
		return 1.0f - x * x;
	}

	ResourceTrade EmbryoGrowingPhysiology::get_new_max_energy_increment(const Embryo &embryo) {
		const NormalizedValue<float> cost =
			std::min(embryo.specie_relative_energy_max(), BASE_VALUE_ENERGY_INCREMENT.value());
		const float gain = cost.value();
		return ResourceTrade{.cost = cost, .gain = gain};
	}

	ResourceTrade EmbryoGrowingPhysiology::get_new_max_life_increment(const Embryo &embryo) {
		const NormalizedValue<float> cost =
			std::min(BASE_VALUE_LIFE_INCREMENT.value(), embryo.specie_relative_life());
		const float gain = cost.value();
		return ResourceTrade{.cost = cost, .gain = gain};
	}

	ResourceTrade EmbryoGrowingPhysiology::get_new_life_increment(const Embryo &embryo) {
		NormalizedValue<float> cost =
			get_increment(embryo.life.ratio(), BASE_VALUE_LIFE_INCREMENT.value());
		float gain = cost.value();
		return ResourceTrade{.cost = cost, .gain = gain};
	}

	ResourceTrade EmbryoGrowingPhysiology::get_new_energy_increment(const Embryo &embryo) {
		NormalizedValue<float> cost =
			get_increment(embryo.energy.ratio(), BASE_VALUE_ENERGY_INCREMENT.value());
		float gain = cost.value();
		return ResourceTrade{.cost = cost, .gain = gain};
	}

	ResourceTrade EmbryoGrowingPhysiology::get_new_health_increment(const Embryo &embryo) {
		NormalizedValue<float> cost =
			EmbryoGrowingPhysiology::get_increment(embryo.health.value(),
												   BASE_VALUE_HEALTH_INCREMENT.value());
		float gain = cost.value();
		return ResourceTrade{.cost = cost, .gain = gain};
	}

	ResourceTrade EmbryoGrowingPhysiology::get_new_area_increment(const Embryo &embryo) {
		NormalizedValue<float> cost = EmbryoGrowingPhysiology::get_increment(
			embryo.area.value / embryo.genome.embryo_genome.morphology.average_area.value()
				* Size::area(embryo.genome.creature_genome.morphology.average_width,
							 embryo.genome.creature_genome.morphology.average_height)
					  .value,
			BASE_VALUE_AREA_INCREMENT);

		float gain = cost.value();
		return ResourceTrade{.cost = cost, .gain = gain};
	}
} // namespace Karkinolution::Organisms::Entities::Embryo::Physiology
