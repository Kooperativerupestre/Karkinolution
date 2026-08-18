#pragma once

#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>

namespace UterusPhysiology {

    NormalizedValue<float> embryotrophy_energy(const Uterus&uterus, const Embryo&embryo);
    EmbryonicEnergy embryotrophy(const Uterus&uterus, const Embryo&embryo, const NormalizedValue<float>&ratio);
    
}


