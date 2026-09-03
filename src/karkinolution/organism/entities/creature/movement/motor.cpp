#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/terrain/entity_map.hpp"
#include <iostream>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/movement/motor.hpp>
#include <karkinolution/world/world.hpp>

MoveOutput MovementMotor::move(Creature& creature, World& world, const Vec3& new_position) {
    if (creature.position == new_position) {
        std::cerr << "Moving a creature to the same position" << "\n";
        return MoveOutput::SAME_POSITION;
    }

    const auto was_updated =
        EntityMapMotor::update_coord(creature.build_id(), world.organism_registry, world.entity_map,
                                     new_position, world.territory);
    if (!was_updated) {
        return MoveOutput::ENTITY_MAP_MOTOR_FAILED;
    }

    creature.position = new_position;
    return MoveOutput::OK;
}
