#include "karkinolution/organism/entities/entities.hpp"

#include <karkinolution/organism/entities/physic/analyzer.hpp>
#include <karkinolution/world/world.hpp>

PhysicAnalyzer::CanBeOutput PhysicAnalyzer::can_be(const Entity &entity, const Karkinolution::World::World &world) {

	auto size     = EntityGetters::get_size(entity);
	auto position = EntityGetters::get_position(entity);


	if (!world.see_entity_map().can_contain(size, position)) {
		return CanBeOutput::OUTSIDE_MAP;
	}

	const auto has_entity = world.see_entity_map().if_find_one_stop(size, position);

	if (has_entity) {
		return CanBeOutput::OVERLAP;
	}

	return CanBeOutput::OK;
}
