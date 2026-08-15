#pragma once
#include <karkinolution/organism/foods/nutrient.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <variant>
#include <karkinolution/organism/stats.hpp>

struct RawMeat {
    float energy;
};

struct Grass {
    float energy;
};

using RawMeatFunction = void (*)(RawMeat&);
using GrassFunction = void (*)(Grass&);
using FoodFunction = std::variant<RawMeatFunction, GrassFunction>;
enum class FoodHint : uint8_t {
    GRASS,
    RAW_MEAT
};