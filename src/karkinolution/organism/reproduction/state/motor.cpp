#include "karkinolution/organism/reproduction/state/physiology.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/reproduction/state/motor.hpp>
#include <karkinolution/organism/reproduction/state/validator.hpp>
#include <karkinolution/organism/reproduction/uterus/motor.hpp>
void ReproductionOrganMotor::run(Creature&creature) {
    ReproductionValidator::has_uterus(creature.reproduction);


    auto uterus = std::get<Uterus>(creature.reproduction.state);

    if (uterus.is_pregnant()) {
        UterusMotor::transfer_energy_to_uterus(creature);
        uterus.get_pregnant_uterus().gestation.pass();
    }
}

ConceiveOutput ReproductionOrganMotor::conceive(Creature &female, Creature &male) {
    ReproductionValidator::has_uterus(female.reproduction);
    auto uterus = std::get<Uterus>(female.reproduction.state);


    if (uterus.is_pregnant()) {
        return ConceiveOutput::ALREADY_PREGNANT;
    }

    int children_count = ReproductionOrganPhysiology::get_children_count(female);
    return ConceiveOutput::OK;
    // add: genome -> embryo
}