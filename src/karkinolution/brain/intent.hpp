#pragma once
#include <karkinolution/brain/planners.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/actions/actions.hpp>
#include <karkinolution/world/world.hpp>
#include <karkinolution/systems/reproductivebuffer.hpp>
#include <karkinolution/actions/presets.hpp>
#include <optional>
#include <karkinolution/brain/perception.hpp>

namespace IntentResolver {
    void to_nothing_intent(Creature&creature);
    void cancel_invalid_intents(Creature&creature);
    void update_intent(Creature&creature, ReproductiveBuffer&reproductive_buffer);
    std::optional<AllPresets> resolve_intent(Creature&creature, World&world,
         const Perception&perception, const DangerIndex&danger_index);
}