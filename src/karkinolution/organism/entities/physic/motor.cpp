#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/organism/entities/physic/analyzer.hpp"
#include "karkinolution/terrain/terrain.hpp"
#include "karkinolution/world/world.hpp"

#include <karkinolution/organism/entities/physic/motor.hpp>

PhysicAnalyzer::CanBeOutput
PhysicMotor::move(EntityId id, const Vec3 &new_position, Karkinolution::World::World &world) {
	auto &entity = world.at_entity(id);

	const auto can_be = Analyzer::can_be(entity, world);

	if (can_be != Analyzer::CanBeOutput::OK) {
		return can_be;
	}


	world.update(EntityGetters::get_id(entity), new_position);
	return Analyzer::CanBeOutput::OK;
}
