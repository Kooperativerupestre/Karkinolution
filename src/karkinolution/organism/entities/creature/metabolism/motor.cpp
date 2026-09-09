#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/metabolism/motor.hpp>
#include <karkinolution/organism/entities/creature/metabolism/physiology.hpp>
#include <karkinolution/organism/foods/foods.hpp>

void MetabolismMotor::eat(Creature &creature, RawMeat &raw_meat) {
	const auto food_division = MetabolismPhysiology::divide(raw_meat.energy);

	const auto energy_to_reach_max   = creature.body.metabolism.energy.remaining();
	const auto r_energy_to_reach_max = creature.body.metabolism.reserved.remaining();

	const auto effective_energy = std::min(energy_to_reach_max, food_division.for_energy);
	const auto effective_reserved_energy =
		std::min(r_energy_to_reach_max, food_division.for_reserved_energy);

	creature.body.metabolism.energy += effective_energy;
	creature.body.metabolism.reserved += effective_reserved_energy;

	raw_meat.energy -= effective_energy + effective_reserved_energy;
}

void MetabolismMotor::eat(Creature &creature, Grass &grass) {
	const auto food_division = MetabolismPhysiology::divide(grass.energy);

	const auto energy_to_reach_max   = creature.body.metabolism.energy.remaining();
	const auto r_energy_to_reach_max = creature.body.metabolism.reserved.remaining();

	const auto effective_energy = std::min(energy_to_reach_max, food_division.for_energy);
	const auto effective_reserved_energy =
		std::min(r_energy_to_reach_max, food_division.for_reserved_energy);

	creature.body.metabolism.energy += effective_energy;
	creature.body.metabolism.reserved += effective_reserved_energy;

	grass.energy -= effective_energy + effective_reserved_energy;
}