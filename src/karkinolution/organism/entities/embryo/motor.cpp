#include "karkinolution/world/motor.hpp"

#include "embryo.hpp"
#include "karkinolution/world/world.hpp"
#include "physiology.hpp"

#include <karkinolution/organism/entities/embryo/motor.hpp>

void EmbryoMotor::grow(Embryo &embryo) {
	const auto volume_trade = EmbryoGrowingPhysiology::get_new_volume_increment(embryo);

	embryo.energy -= volume_trade.cost;
	embryo.volume.value += volume_trade.gain;

	const auto life_trade = EmbryoGrowingPhysiology::get_new_life_increment(embryo);

	embryo.energy -= life_trade.cost;
	embryo.life += life_trade.gain;

	const auto max_life_trade = EmbryoGrowingPhysiology::get_new_max_life_increment(embryo);

	embryo.energy -= max_life_trade.cost;
	embryo.life.max_ref() += max_life_trade.gain;

	const auto max_energy_trade = EmbryoGrowingPhysiology::get_new_max_energy_increment(embryo);

	embryo.energy -= max_energy_trade.cost;
	embryo.energy.max_ref() += max_energy_trade.gain;

	const auto health_trade = EmbryoGrowingPhysiology::get_new_health_increment(embryo);

	embryo.energy -= health_trade.cost;
	embryo.health += health_trade.gain;
}

void EmbryoMotor::run(Embryo &embryo, World &world) {
	if (EmbryoPhysiology::is_dead(embryo)) {
		WorldMotor::remove(embryo.build_id(), world);
	}

	grow(embryo);

	embryo.age++;
}
