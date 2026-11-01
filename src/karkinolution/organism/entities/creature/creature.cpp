#include <karkinolution/math/global_epsilon.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution::Organisms::Entities::Creature {

/*
[[nodiscard]] NormalizedValue<float> Body::senescence() const noexcept {
	return NormalizedValue<float>{static_cast<float>(age.value()/(std::pow(age.max(), 1.4)))};
}


NormalizedValue<float> Body::reproductive_maturity() const {
	return static_cast<float>(std::exp(std::pow(std::pow(-(-age.ratio().value() - 0.45)
							, 2)/(0.2), 2)));
}
							*/


[[nodiscard]] NormalizedValue<float> MetabolismPart::hungry() const {
	return energy.ratio();
}

[[nodiscard]] Karkinolution::Math::Units::Mass MorphologyPart::density() const noexcept {
	if (volume().value == Math::Approx<float>(0.0f)) {
		return 0.0f;
	}
	return mass.value / volume().value;
}

[[nodiscard]] Volume MorphologyPart::volume() const noexcept {
	return size.volume();
}

[[nodiscard]] Strength Creature::strength() const noexcept {
	float specie_strength = 0.0f;
	switch (genome.core_genome.specie) {
		case CreatureSpecies::HIPPOPOTAMUS:
		case CreatureSpecies::CROCODILE:
			specie_strength = 35.0f;
			break;
		case CreatureSpecies::CRAB:
			specie_strength = 15.0f;
			break;
		case CreatureSpecies::FISH:
			specie_strength = 3.0f;
			break;
	}
	return Strength{specie_strength * body.vital.life.ratio().value()};
}

[[nodiscard]] float Creature::specie_relative_max_energy() const noexcept {
	return body.metabolism.energy.max() / genome.creature_genome.metabolism.max_energy;
}

[[nodiscard]] float Creature::specie_relative_max_reserved_energy() const noexcept {
	return body.metabolism.reserved.max() / genome.creature_genome.metabolism.max_reserved_energy;
}

[[nodiscard]] float Creature::specie_relative_mass() const noexcept {
	return body.morphology.mass.value / genome.creature_genome.morphology.average_mass.value;
}

[[nodiscard]] float Creature::specie_relative_volume() const noexcept {
	return body.morphology.volume().value
		/ Size::volume(genome.creature_genome.morphology.average_lateral,
					   genome.creature_genome.morphology.average_height,
					   genome.creature_genome.morphology.average_back)
			  .value;
}

[[nodiscard]] float Creature::specie_relative_age() const noexcept {
	return body.age.value / genome.creature_genome.vital.average_age.value;
}

} // namespace Karkinolution::Organisms::Entities::Creature

// All of this average values (genome) must be > 0
