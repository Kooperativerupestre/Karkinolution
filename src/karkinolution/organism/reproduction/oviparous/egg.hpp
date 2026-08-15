#pragma once
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/organism/foods/nutrient.hpp>
#include <karkinolution/math/units.hpp>
#include <limits>
#include <vector>

using PhysicsStats::Volume;
using OrganismStats::Health::Health;

class ShellValue : RuntimeLimitedValue<ShellValue, float> {
    using RuntimeLimitedValue<ShellValue, float>::RuntimeLimitedValue;
};

struct Shell {
    NormalizedValue<float> resistance;
    ShellValue state;
    Volume volume;
};

struct EggNutrients {
    Nutrient nutrient;
    Health health;
};


struct Egg {
    Shell shell;
    EggNutrients nutrients;
    std::vector<Id> embryos;
};

