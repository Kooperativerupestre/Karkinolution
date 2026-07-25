#include <karkinolution/brain/perception.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/systems/reproduction.hpp>
#include <karkinolution/systems/physics.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/actions/presets.hpp>
#include <optional>
#include <karkinolution/world/world.hpp>
#include <string>
#include <karkinolution/resolvers/resolvers.hpp>
#include <vector>
#include <karkinolution/utils/k_random.hpp>
#include <format>

// ReproductionResolver

std::optional<Id> ReproductionResolver::find_adjacente_mates(const Creature& creature, const Perception& perception) {
    std::vector<Id> ids;

    auto neighbors = PerceptionAnalyser::neighbors_8(perception);

    for (const auto& [c, b] : neighbors.pieces_ref()) {
        if (b.has_creature() && ReproductiveSystem::can_reproduce(creature, b.get_creature())) {
            ids.push_back(b.get_creature().id);
        }
    }

    if (ids.size() == 0) {
        return std::nullopt;
    }
    return Choices::choice(ids);
}

Parents ReproductionResolver::resolve_parents(const Creature& A, Id B_id, const EntitiesRegistry& entities) {
    const Creature* B = &entities.at_creature(B_id);


    return ReproductiveSystem::return_parents(A, *B);
}

ReproducePreset ReproductionResolver::mate_to_preset(const Parents& parents) {
    return ReproducePreset{parents.female_id, parents.male_id};
}

std::optional<ReproducePreset> ReproductionResolver::resolve_reproduction(const Creature& creature, const Perception& perception, const EntitiesRegistry& entities) {
    std::optional<Id> id_mate = ReproductionResolver::find_adjacente_mates(creature, perception);

    if (!id_mate.has_value()) {
        return std::nullopt;
    }
    return ReproductionResolver::mate_to_preset(ReproductionResolver::resolve_parents(creature, id_mate.value(), entities));
}

// --- BornResolver ---

Creature BornResolver::born_data_to_creature(const BornData& born_data, Vec2 coord) {
    return CreatureFactory::gen_creature(
        CreatureConfig{
            .position = coord,
            .specie = born_data.genome.core.specie,
            .initial_energy = born_data.initial_energy,
            .genome = std::move(born_data.genome),
            .sociability = born_data.sociability
        }
    );
}

std::optional<Creature> BornResolver::resolve_born_data(
    const std::optional<BornData>& born_data_opt,
    Vec2 position,
    World& world,
    const std::string& mother_name
) {
    if (!born_data_opt.has_value()) {
        world.add_log(std::format("one child of {} was died", mother_name));
        return std::nullopt;
    }

    Creature new_child = BornResolver::born_data_to_creature(
        *born_data_opt,
        position
    );

    world.add_log(std::format("child {} of creature {} born", new_child.name, mother_name));

    return new_child;
}