#include "karkinolution/organism/entities/embryo/motor.hpp"
#include "karkinolution/terrain/motor.hpp"

#include <karkinolution/lifecycle/lifecycle.hpp>

void LifeCycle::run(Entity &entity, World &world) {
	if (std::holds_alternative<Embryo>(entity)) {
		EmbryoMotor::run(std::get<Embryo>(entity), world);
	} else if (std::holds_alternative<Creature>(entity)) {
		CreatureMotor::run(std::get<Creature>(entity), world);
	} else if (std::holds_alternative<Corpse>(entity)) {
		CorpseMotor::run(std::get<Corpse>(entity), world);
	} else if (std::holds_alternative<Egg>(entity)) {
		return;
		// TODO
	}
}

void LifeCycle::run(World &world) {
	world.pass_time();
	for (auto &piece : world.territory.soils()) {
		SoilPieceMotor::run(piece);
	}

	for (auto &entity : world.organism_registry.entities.values()) {
		run(entity, world);
	}
}