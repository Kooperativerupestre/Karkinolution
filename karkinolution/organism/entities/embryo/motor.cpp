#include "embryo.hpp"
#include "karkinolution/world/world.hpp"
#include "physiology.hpp"

#include <karkinolution/organism/entities/embryo/motor.hpp>

namespace Karkinolution::Organisms::Entities::Embryo::Motor {
	void EmbryoMotor::grow(Embryo &embryo) {
		const auto area_trade =
			Karkinolution::Organisms::Entities::Embryo::Physiology::EmbryoGrowingPhysiology::get_new_area_increment(
				embryo);

		embryo.energy -= area_trade.cost;
		embryo.area.value += area_trade.gain;

		const auto life_trade =
			Karkinolution::Organisms::Entities::Embryo::Physiology::EmbryoGrowingPhysiology::get_new_life_increment(
				embryo);

		embryo.energy -= life_trade.cost;
		embryo.life += life_trade.gain;

		const auto max_life_trade =
			Karkinolution::Organisms::Entities::Embryo::Physiology::EmbryoGrowingPhysiology::get_new_max_life_increment(
				embryo);

		embryo.energy -= max_life_trade.cost;
		embryo.life.max_ref() += max_life_trade.gain;

		const auto max_energy_trade = Karkinolution::Organisms::Entities::Embryo::Physiology::EmbryoGrowingPhysiology::
			get_new_max_energy_increment(embryo);

		embryo.energy -= max_energy_trade.cost;
		embryo.energy.max_ref() += max_energy_trade.gain;

		const auto health_trade =
			Karkinolution::Organisms::Entities::Embryo::Physiology::EmbryoGrowingPhysiology::get_new_health_increment(
				embryo);

		embryo.energy -= health_trade.cost;
		embryo.health += health_trade.gain;
	}

	void EmbryoMotor::run(Embryo &embryo, Karkinolution::World::World &world) {
		if (Karkinolution::Organisms::Entities::Embryo::Physiology::EmbryoPhysiology::is_dead(embryo)) {
			world.remove(embryo.build_id());
		}

		grow(embryo);

		embryo.age++;
	}
} // namespace Karkinolution::Organisms::Entities::Embryo::Motor
