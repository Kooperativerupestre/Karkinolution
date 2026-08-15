
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/core/global_epsilon.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
/*
NormalizedValue<float> UterusPhysiology::get_mortality(const Uterus& uterus, const Body& body, const Embryo&embryo) {
    const NormalizedValue<float> senescence_factor = body.senescence()/2;
    const NormalizedValue<float> vital_factor = (
        (1.0f - body.vital.health.value())  +
        (1.0f - body.vital.life.value()) +
        (1.0f - body.vital.imunity.value()))/4.0f;
        
    const PregnantUterus* p_uterus = &uterus.get_pregnant_uterus();
    NormalizedValue<float> gestation_progress_factor = 0;

    if (p_uterus->gestation.is_below_max()) {
        gestation_progress_factor += p_uterus->gestation.remaining_to_max()/p_uterus->gestation.max();
    } else if (p_uterus->gestation.is_above_max()) {
        gestation_progress_factor += p_uterus->gestation.remaining_to_max()*1.3/p_uterus->gestation.max();
    }
    
    
    return senescence_factor + (vital_factor * gestation_progress_factor) - (embryo.health.value() * 1.5);
}*/

NormalizedValue<float> UterusPhysiology::birth_risk(const Uterus& uterus, const Body& body, const Embryo&embryo) {
    const NormalizedValue<float> vital_factor = (
        (1.0f - body.vital.health.value())*0.75  +
        (1.0f - body.vital.life.value())*1.25 +
        (1.0f - body.vital.immunity.value()))/3.0f;
    
    const PregnantUterus* p_uterus = &uterus.get_pregnant_uterus();
    NormalizedValue<float> gestation_progress_factor;

    if (p_uterus->gestation.is_below_max()) {
        gestation_progress_factor += p_uterus->gestation.remaining_to_max()*1.45/p_uterus->gestation.max();
    } else if (p_uterus->gestation.is_above_max()) {
        gestation_progress_factor += p_uterus->gestation.remaining_to_max()/p_uterus->gestation.max();
    }
    return vital_factor * gestation_progress_factor - (embryo.life.value()*1.5);
}

/*NormalizedValue<float> UterusPhysiology::pregnancy_gravity(const Uterus& uterus, const Body& body, int average_specie_children_count) {
    Validator::validate_pregnant_uterus(uterus);

    NormalizedValue<float> age_factor = body.senescence();
    age_factor*=age_factor;

    const PregnantUterus* p_uterus = &uterus.get_pregnant_uterus();
    
    return p_uterus->gestation.progress() * NormalizedValue<float>(static_cast<float>(p_uterus->embryos_count())/average_specie_children_count);
}
    */

NormalizedValue<float> UterusPhysiology::embryotrophy_energy(const Uterus& uterus, const Embryo& embryo) {
    float dist = embryo.hungry().ratio() - 0.85; // max = 0.15 (1 - 0.85)
    return RandomGenerators::generate(0.85 - dist, 1.0) / uterus.get_pregnant_uterus().embryos_count() + dist;
}
