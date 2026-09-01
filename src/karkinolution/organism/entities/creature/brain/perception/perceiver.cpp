#include "karkinolution/core/error.hpp"
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/terrain/terrain.hpp"
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perceiver.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/world.hpp>

PerceivedCreature Perceiver::perceive(const Creature& target, const Creature& perceiver) {
    PerceivedCreature perceived = {.id = target.build_id(),
                                   .energy = target.body.metabolism.energy,
                                   .life = target.body.vital.life,
                                   .gender = target.ontology.gender,
                                   .position = target.position,
                                   .size = target.body.morphology.size};
    return perceived;
}

PerceivedCorpse Perceiver::perceive(const Corpse& target, const Creature& perceiver) {
    PerceivedCorpse perceived = {.id = target.build_id(),
                                 .meat = target.raw_meat,
                                 .position = target.position,
                                 .size = target.size};
    return perceived;
}

PerceivedSoil Perceiver::perceive(const SoilPiece& target, const Creature& perceiver) {
    PerceivedSoil perceived = {.radius = target.radius};

    if (target.components.exists<SoilPieceComponents::FoodState>()) {
        perceived.food = target.components.try_get<SoilPieceComponents::FoodState>()->food;
    }

    if (target.components.exists<SoilPieceComponents::Damage>()) {
        perceived.damage = target.components.try_get<SoilPieceComponents::Damage>()->damage;
    }

    if (target.components.exists<SoilPieceComponents::MovementCost>()) {
        perceived.movement_cost =
            target.components.try_get<SoilPieceComponents::MovementCost>()->cost;
    }
    perceived.id = target.id;
    perceived.position = target.position;
    return perceived;
}

Perception Perceiver::perceive(const Creature& perceiver, const World& world) {
    PerceptionEntityRegistry p_entities;
    PerceptionSoilRegistry p_soils;
    PerceptionData data;

    const Radius& radius = perceiver.genome.core_genome.vision_radius;

    const auto soils_id = TerritoryMotor::find(radius, perceiver.position, world.territory);
    const auto entities_id = EntityMapMotor::find(radius, perceiver.position, world.entity_map);

    p_entities.reserve(entities_id.size());
    p_soils.reserve(soils_id.size());

    for (auto id : soils_id) {
        const auto& soil = world.soil_registry.at(id);
        p_soils.add(id, perceive(soil, perceiver));
        data.insert(id, BoxConversion::to_box(soil));
    }

    for (auto id : entities_id) {
        if (id.type == EntityTypes::EGG) {
            throw SimulationError("It was not implemented the perceive of egg");
        } else if (id.type == EntityTypes::EMBRYO) {
            throw SimulationError("It was not implemented the perceive of embryo");
        }
        const auto& entity = world.organism_reistr.entities.at(id);
        if (id.type == EntityTypes::CREATURE) {

            p_entities.add(id, perceive(std::get<Creature>(entity), perceiver));
        } else { // == Corpse
            p_entities.add(id, perceive(std::get<Corpse>(entity), perceiver));
        }
        data.insert(id, BoxConversion::to_box(EntityGetters::get_size(entity),
                                              EntityGetters::get_position(entity)));
    }
    return Perception{data, radius, p_entities, p_soils, perceiver.position};
}