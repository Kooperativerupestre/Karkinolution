#pragma once
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/world/world.hpp>
#include <karkinolution/resolvers/resolvers.hpp>
#include <karkinolution/brain/perception.hpp>
#include <optional>
#include <karkinolution/actions/presets.hpp>
#include <vector>

namespace TickResolvers {
    void resolve_death(Creature&creature, World&wolrd);
    std::optional<Creature> resolve_born(Creature&creature, const Perception&perception, World&world);
}

namespace RunnerCreature {
    void run_basic_fisiology(Creature&creature);
    std::optional<Creature> run_uterus(Creature&creature, const Perception&perception, World&world);
    std::optional<ReproducePreset> try_to_get_reproduce_preset(Creature&creature, const Perception&perception, World&world);
    std::vector<AllPresets> get_presets(Creature&creature, const Perception&perception,
                                        const DangerIndex&danger_index, World&world);
    void run_creature(Creature&creature, World&world);
}
namespace RunnerCorpse {
    void to_degrade_corpse(Corpse&corpse);
    void run_corpse(Corpse&corpse, World&world);
}