#pragma once
#include <cstdint>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

enum class ConceiveOutput : uint8_t {
    ALREADY_PREGNANT,
    OK
};

namespace ReproductionOrganMotor {
    void run(Creature&creature);
    ConceiveOutput conceive(Creature&, Creature&);
}