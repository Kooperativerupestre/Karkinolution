#include "karkinolution/organism/reproduction/state/motor.hpp"
#include <karkinolution/organism/entities/creature/brain/perception/perceiver.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/motor.hpp>
#include <karkinolution/organism/entities/creature/physiology.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/world/motor.hpp>
#include <karkinolution/world/world.hpp>

void CreatureMotor::grow(Creature& creature, const OrganismRegistry& organisms) {
    auto& body = creature.body;
    auto muscle_grow = CreatureGrowingPhysiology::get_new_muscle_increment(creature, organisms);

    body.muscle.muscle.value += muscle_grow.gain;
    body.metabolism.reserved -= muscle_grow.cost.reserved_energy;

    auto skeleton_grow = CreatureGrowingPhysiology::get_new_skeleton_increment(creature, organisms);

    body.skeleton.bone.value += skeleton_grow.gain;
    body.metabolism.reserved -= skeleton_grow.cost.reserved_energy;

    auto lateral_grow = CreatureGrowingPhysiology::get_new_lateral_increment(creature, organisms);
    auto depth_grow = CreatureGrowingPhysiology::get_new_depth_increment(creature, organisms);
    auto height_grow = CreatureGrowingPhysiology::get_new_height_increment(creature, organisms);

    body.morphology.size.height.value += height_grow;
    body.morphology.size.back.value += depth_grow;
    body.morphology.size.lateral.value += lateral_grow;

    auto max_energy_grow =
        CreatureGrowingPhysiology::get_new_max_energy_increment(creature, organisms);
    body.metabolism.energy.max_ref() += max_energy_grow.gain;
    body.metabolism.reserved -= max_energy_grow.cost.reserved_energy;

    auto max_reserved_grow =
        CreatureGrowingPhysiology::get_new_max_energy_reserved_increment(creature, organisms);
    body.metabolism.reserved.max_ref() += max_reserved_grow.gain;
    body.metabolism.reserved -= max_reserved_grow.cost.reserved_energy;
}

void CreatureMotor::run(Creature& creature, World& world) {
    // aliases

    auto& organisms = world.organism_registry;

    // code
    if (CreaturePhysiology::is_dead(creature)) {
        WorldMotor::remove(creature.build_id(), world);
    }

    const auto age_effects = CreaturePhysiology::get_to_age_effects(creature);
    auto& body = creature.body;
    age_effects(creature);
    grow(creature, organisms);

    if (creature.reproduction.is_pregnant()) {
        ReproductionStateMotor::run(creature);
    }
    world.entity_map.root().update(
        creature.build_id(),
        AABBConversion::to_aabb(creature.body.morphology.size, creature.position));

    const Perception perception = Perceiver::perceive(creature, world);
}
