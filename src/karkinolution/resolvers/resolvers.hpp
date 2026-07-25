#pragma once
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/systems/reproduction.hpp>
#include <karkinolution/systems/physics.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/actions/presets.hpp>
#include <optional>
#include <karkinolution/world/world.hpp>
#include <string>

namespace ReproductionResolver {
    std::optional<Id> find_adjacente_mates(const Creature&creature, const Perception&perception);
    Parents resolve_parents(const Creature&A, Id B_id, const EntitiesRegistry&entities);
    // Removed choose_mate: unnecessary wrapper

    ReproducePreset mate_to_preset(const Parents& parent);
    std::optional<ReproducePreset> resolve_reproduction(const Creature&creature, const Perception&perception, const EntitiesRegistry&entities);
}

namespace BornResolver {
    Creature born_data_to_creature(const BornData&born_data, Vec2 coord);
    std::optional<Creature> resolve_born_data(const std::optional<BornData>&, Vec2 position, World&world, const std::string&mother_name);
}
