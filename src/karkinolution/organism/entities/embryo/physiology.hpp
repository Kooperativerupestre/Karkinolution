#pragma once

#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>

// Energy 100% (embryotrophic)

// ratio = 0.5 -> base_value
// ratio = 0 -> base_value * 2
// ratio = 1 -> 0

// Reserved for new_life_increment -> 30%
// Reserved for new_energy_increment -> 30%
// Reserved for new_health_increment -> 10%
// Reserved for new_max_energy_increment -> 10%
// Reserved for new_max_life_increment -> 20%

inline constexpr NormalizedValue<float> BASE_VALUE_LIFE_INCREMENT{0.3f};
inline constexpr NormalizedValue<float> BASE_VALUE_ENERGY_INCREMENT{0.3f};
inline constexpr NormalizedValue<float> BASE_VALUE_HEALTH_INCREMENT{0.1f};
inline constexpr NormalizedValue<float> BASE_VALUE_MAX_LIFE_INCREMENT{0.1f};
inline constexpr NormalizedValue<float> BASE_VALUE_MAX_ENERGY_INCREMENT{0.2f};
inline constexpr NormalizedValue<float> BASE_VALUE_VOLUME_INCREMENT{0.1f};

inline constexpr float EMBRYO_BASAL_METABOLISM = 10.0f;

using OrganismStats::Energy;


struct ResourceTrade {
    const NormalizedValue<float> cost;
    const float gain;
};

namespace EmbryoGrowingPhysiology {
    constexpr NormalizedValue<float> get_increment(const NormalizedValue<float>& ratio, const NormalizedValue<float>& factor);
    
    ResourceTrade get_new_max_energy_increment(const Embryo&embryo, const Energy&energy);
    ResourceTrade get_new_max_life_increment(const Embryo&embryo, const Energy&energy);

    ResourceTrade get_new_life_increment(const Embryo&embryo, const Energy&energy);
    ResourceTrade get_new_energy_increment(const Embryo&embryo, const Energy&energy);
    ResourceTrade get_new_health_increment(const Embryo&embryo, const Energy&energy);
    ResourceTrade get_new_volume_increment(const Embryo&embryo, const Energy&energy);
}

namespace EmbryoPhysiology {
    bool is_dead(const Embryo&embryo);
    float basal_metabolism(const Embryo&embryo);

    float transfor_into_nutrients(const Embryo&embryo);
}
