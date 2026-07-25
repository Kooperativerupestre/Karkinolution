#include "karkinolution/organism/ontology.hpp"
#include <karkinolution/brain/instincts/factors.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/organism/validator.hpp>

namespace Factors {

    NormalizedValue get_pregnancy_risk(const Creature& creature) {
        NormalizedValue factor{};

        Validator::validate_pregnant(creature);

        const PregnantUterus* uterus = &std::get<PregnantUterus>(creature.uterus);

        factor += uterus->gravity() * 0.75f;
        factor -= uterus->children_count.ratio() * 0.35f;
        return factor;
    }

    NormalizedValue get_fear(const Creature& creature) {
        NormalizedValue factor{};

        NormalizedValue ph_r = creature.physical_ratio();
        if (ph_r.value() < 0.30) {
            factor += ph_r - 0.05;
        }

        if (creature.pregnant()) {
            factor += std::get<PregnantUterus>(creature.uterus).gravity();
        }
        factor += creature.senescence()/1.7;
        return factor;
    }

    NormalizedValue get_courage(const Creature& creature) {
        NormalizedValue basal{COURAGE_FACTOR.at(creature.genome.core.temperament)};
        basal -= creature.senescence();
        basal += creature.genome.metabolism.mass/3;
        return basal;
    }

    NormalizedValue get_territorial_indifference_factor(const Creature& creature) {
        return creature.genome.core.temperament == Temperament::TERRITORIAL ? 0.54f : 1.0f;
    }

    NormalizedValue get_pregnant_social_interest_factor(const Creature& creature) {
        return creature.pregnant() ? 1.2f : 1.0f;
    }

    NormalizedValue get_pregnant_territorial_avoidance_attack_factor(const Creature& creature) {
        Validator::validate_pregnant(creature);
        return (std::get<PregnantUterus>(creature.uterus).gravity().value() + 1)  * 0.2;
    }

}