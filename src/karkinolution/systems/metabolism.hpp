#pragma once
#include <karkinolution/core/stats.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <vector>
#include <karkinolution/brain/perception.hpp>

inline constexpr float CORPSE_FOOD_YIELD = 0.8f;

struct FoodOption {
    const Vec2 coord;
    const FoodHint hint;
    const float energy_gain;
};

namespace MetabolismSystem {
    float get_food_yield(float energy, const FoodHint&food_hint);
    std::vector<FoodOption> get_food_options(const Perception&perception, Vec2 coord);
    float diet_effective(float diet_bias, NormalizedValue hungry);
    void eat(Creature&creature, Energy&energy, const FoodHint&food_hint);
}
