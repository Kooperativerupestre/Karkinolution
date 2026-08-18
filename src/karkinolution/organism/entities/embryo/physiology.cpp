#include "karkinolution/organism/stats.hpp"
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/embryo/physiology.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <algorithm>
#include <karkinolution/math/stats/compile_values.hpp>

bool EmbryoPhysiology::is_dead(const Embryo& embryo) {
    return embryo.life.is_zero() || embryo.energy.is_zero();
}

float EmbryoPhysiology::basal_metabolism(const Embryo &embryo) {
    return EMBRYO_BASAL_METABOLISM;
}


constexpr NormalizedValue<float> EmbryoGrowingPhysiology::get_increment(const NormalizedValue<float>& ratio, const NormalizedValue<float>& factor) {
    if (ratio < Approx<float>(0.5)) {
        float x = (0.5f - ratio.value())/0.5f;
        return 1.0f + x * x;
    }
    float x = (ratio.value() - 0.5f) / 0.5;
    return 1.0f - x * x;
}

ResourceTrade EmbryoGrowingPhysiology::get_new_max_energy_increment(const Embryo& embryo, const Energy& energy) {
    NormalizedValue<float> cost = std::min(embryo.remaining_growth_capacity().value(), BASE_VALUE_ENERGY_INCREMENT.value());
    float gain = cost.value() * energy.value();
    return ResourceTrade{
        .cost = cost,
        .gain = gain
    };
}

ResourceTrade EmbryoGrowingPhysiology::get_new_max_life_increment(const Embryo& embryo, const Energy& energy) {
    NormalizedValue<float> cost = std::min(BASE_VALUE_LIFE_INCREMENT.value(), embryo.remaining_growth_capacity().value());
    float gain = cost.value() * energy.value();
    return ResourceTrade{
        .cost = cost,
        .gain = gain
    };
}

ResourceTrade EmbryoGrowingPhysiology::get_new_life_increment(const Embryo& embryo, const Energy& energy) {
    NormalizedValue<float> cost = get_increment(embryo.life.ratio(), std::min(embryo.remaining_growth_capacity().value(), BASE_VALUE_LIFE_INCREMENT.value()));
    float gain = cost.value() * energy.value();
    return ResourceTrade{
        .cost = cost,
        .gain = gain
    };
}

ResourceTrade EmbryoGrowingPhysiology::get_new_energy_increment(const Embryo& embryo, const Energy& energy) {
    NormalizedValue<float> cost = get_increment(embryo.energy.ratio(), std::min(BASE_VALUE_ENERGY_INCREMENT.value(), embryo.remaining_growth_capacity().value()));
    float gain = cost.value() * energy.value();
    return ResourceTrade{
        .cost = cost,
        .gain = gain
    };
}

ResourceTrade EmbryoGrowingPhysiology::get_new_health_increment(const Embryo& embryo, const Energy& energy) {
    NormalizedValue<float> cost = EmbryoGrowingPhysiology::get_increment(embryo.health.value(), std::min(BASE_VALUE_HEALTH_INCREMENT.value(), embryo.remaining_growth_capacity().value()));
    float gain = cost.value() * energy.value();
    return ResourceTrade{
        .cost = cost,
        .gain = gain
    };
}

ResourceTrade EmbryoGrowingPhysiology::get_new_volume_increment(const Embryo &embryo, const Energy&energy) {
    NormalizedValue<float> cost = EmbryoGrowingPhysiology::get_increment(
        embryo.volume.value/embryo.genome.embryo_genome.morphology.average_volume.value()
        * Size::volume(
            embryo.genome.creature_genome.morphology.average_lateral,
            embryo.genome.creature_genome.morphology.average_height,
            embryo.genome.creature_genome.morphology.average_back
        ).value, BASE_VALUE_VOLUME_INCREMENT);

    float gain = cost.value() * energy.value();
    return ResourceTrade{
        .cost = cost,
        .gain = gain
    };
}