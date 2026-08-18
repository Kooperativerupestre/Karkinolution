#pragma once
#include "karkinolution/organism/reproduction/state/state.hpp"
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>

namespace UterusMotor {
    void transfer_energy_to_uterus(Creature&creature);
    void run(Creature&creature);
}