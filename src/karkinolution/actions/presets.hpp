#pragma once
#include <karkinolution/core/stats.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/core/vec2.hpp>
#include <variant>

struct MovePreset {
    Vec2 new_coord;
};

struct ReproducePreset {
    Id female, male;
};

struct AttackPreset {
    Id target;
};

struct EatPreset {
    Energy& energy;
    FoodHint food_hint;
};

using AllPresets = std::variant<MovePreset, EatPreset, ReproducePreset, AttackPreset>;