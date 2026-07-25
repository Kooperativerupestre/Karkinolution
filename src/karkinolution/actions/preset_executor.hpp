#pragma once
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/systems/physics.hpp>
#include <karkinolution/systems/metabolism.hpp>
#include <karkinolution/actions/presets.hpp>
#include <karkinolution/world/world.hpp>

enum class MoveOutputs : uint8_t {
    OK,
    INSUFFICIENT_ENERGY,
    CANNOT_GET_BEST_POSITION
};

namespace PresetExecutor {
    void execute_preset(const ReproducePreset&preset, World&world);
    void execute_preset(const EatPreset&preset, Creature&creature, World&world);
    MoveOutputs execute_preset(const MovePreset&preset, Creature&creature,
                                const Perception&perception, const DangerIndex&danger_index,
                                World&world);

    void execute_preset(const AttackPreset&preset, Creature&creature, World&world);  

    void execute_generic_preset(const AllPresets&preset, Creature&creature, 
        const Perception&perception, const DangerIndex&danger_index, World&world);
}