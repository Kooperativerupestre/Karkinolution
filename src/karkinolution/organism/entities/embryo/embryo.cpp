#include <karkinolution/organism/entities/embryo/embryo.hpp>

EmbryoHungry Embryo::hungry() const {
	return (1.0f - life.ratio()) + (1.0f - energy.ratio()) + (1.0f - health.ratio() / 2.0f);
}

[[nodiscard]] float Embryo::specie_relative_energy_max() const {
	return energy.max() / genome.calculate_embryo_average_max_energy();
}

[[nodiscard]] double Embryo::specie_relative_volume() const {
	return volume.value / genome.calculate_embryo_average_volume().value;
}

[[nodiscard]] float Embryo::specie_relative_life() const {
	return life.max() / genome.calculate_embryo_average_life();
}