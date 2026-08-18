#include "karkinolution/organism/reproduction/state/state.hpp"
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/reproduction/uterus/motor.hpp>
#include <karkinolution/organism/reproduction/state/validator.hpp>

void UterusMotor::transfer_energy_to_uterus(Creature &creature) {
    ReproductionValidator::is_pregnant(creature.reproduction);
    auto uterus = std::get<Uterus>(creature.reproduction.state);


    auto uterus_hungry = uterus.hungry();

    float energy_trade = std::min(creature.body.metabolism.energy.value() * 0.2f, uterus_hungry.value() * uterus.energy.max());
    float reserved_trade = std::min(creature.body.metabolism.reserved.value() * 0.5f, uterus_hungry.value() * uterus.energy.max());

    creature.body.metabolism.energy -= energy_trade;
    creature.body.metabolism.reserved -= reserved_trade;

    uterus.energy += energy_trade;
    uterus.energy += reserved_trade;
}




