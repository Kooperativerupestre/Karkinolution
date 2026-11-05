#include "karkinolution/organism/entities/embryo/motor.hpp"
#include "karkinolution/terrain/soil/motor.hpp"

#include <karkinolution/lifecycle/lifecycle.hpp>

namespace Karkinolution::Lifecycle {

	void run(Karkinolution::Organisms::Entities::Entity &entity,
			 Karkinolution::World::World                &world) {
		if (std::holds_alternative<Karkinolution::Organisms::Entities::Embryo::Embryo>(entity)) {
			Karkinolution::Organisms::Entities::Embryo::Motor::EmbryoMotor::run(
				std::get<Karkinolution::Organisms::Entities::Embryo::Embryo>(entity),
				world);
		} else if (std::holds_alternative<Karkinolution::Organisms::Entities::Creature::Creature>(
					   entity)) {
			Karkinolution::Organisms::Entities::Creature::Motor::CreatureMotor::run(
				std::get<Karkinolution::Organisms::Entities::Creature::Creature>(entity),
				world);
		} else if (std::holds_alternative<Karkinolution::Organisms::Entities::Corpse::Corpse>(
					   entity)) {
			Karkinolution::Organisms::Entities::Corpse::Motor::run(
				std::get<Karkinolution::Organisms::Entities::Corpse::Corpse>(entity),
				world);
		} else if (std::holds_alternative<Karkinolution::Organisms::Entities::Egg::Egg>(entity)) {
			return;
			// TODO
		}
	}

	void run(Karkinolution::World::World &world) {
		world.pass_time();
		for (auto &entry : world.see_territory().registry()) {
			Karkinolution::Terrain::Soil::Motor::SoilPieceMotor::run(entry.second);
		}

		std::vector<Karkinolution::Organisms::Entities::EntityId> entity_ids;
		for (const auto id : world.see_entities_registry().keys()) {
			entity_ids.push_back(id);
		}

		for (const auto id : entity_ids) {
			if (world.see_entities_registry().exists(id)) {
				run(world.at_entity(id), world);
			}
		}

	} // namespace Karkinolution::Lifecycle
} // namespace Karkinolution::Lifecycle
