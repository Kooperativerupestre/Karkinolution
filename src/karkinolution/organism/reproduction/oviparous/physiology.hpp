#pragma once
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/reproduction/oviparous/egg.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>


namespace EggPhysiology {
    EmbryonicEnergy embryotrophy(const Egg&egg, const Embryo&embryo, const NormalizedValue<float>&ratio);
}