#include <karkinolution/organism/entities/embryo/embryo.hpp>

namespace Karkinolution::Organisms::Entities::Embryo {

	EmbryoHungry Embryo::hungry() const {
		return (1.0f - life.ratio().value()) + (1.0f - energy.ratio().value())
			+ (1.0f - health.value() / 2.0f);
	}

	[[nodiscard]] float Embryo::specie_relative_energy_max() const {
		return energy.max() / genome.calculate_embryo_average_max_energy();
	}

	[[nodiscard]] double Embryo::specie_relative_area() const {
		return area.value / genome.calculate_embryo_average_area().value;
	}

	[[nodiscard]] float Embryo::specie_relative_life() const {
		return life.max() / genome.calculate_embryo_average_life();
	}

} // namespace Karkinolution::Organisms::Entities::Embryo
