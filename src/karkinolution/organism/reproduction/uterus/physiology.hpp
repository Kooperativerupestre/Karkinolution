#pragma once

#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>

namespace UterusPhysiology {
    NormalizedValue<float> get_mortality(const Uterus&uterus, const Body&body, const Embryo&embryo);      
    NormalizedValue<float> birth_risk(const Uterus&uterus, const Body&body, const Embryo&embryo);
    NormalizedValue<float> pregnancy_gravity(const Uterus&uterus, const Body&body, int average_specie_children_count);
    NormalizedValue<float> embryotrophy_energy(const Uterus&uterus, const Embryo&embryo);
    EmbryonicEnergy embryotrophy(const Uterus&uterus, const Embryo&embryo, const NormalizedValue<float>&ratio);
    

    // mother -> uterus
    // mother <- uterus

    float transfer_nutrients(const Creature&creature, const Uterus&uterus);
    float pregnancy_cost(const Creature&creature, const Uterus&uterus, const EntitiesRegistry&entities);
}


