
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/pathogens/virus/virus.hpp>
#include <karkinolution/organism/pathogens/virus/motor.hpp>
#include <karkinolution/utils/k_random.hpp>

// Apply
void VirusMotor::apply(Creature&creature, const Vorax&virus) {
    creature.body.muscle.muscle.value *= virus.consume_muscles.value();
    creature.body.metabolism.reserved *= virus.consume_metabolism.value();
    creature.body.metabolism.energy *= virus.consume_metabolism.value()/2.0f;
}

void VirusMotor::apply(Embryo&embryo, const Vorax&virus) {
    embryo.energy *= virus.consume_metabolism.value();
    embryo.life *= virus.consume_muscles.value()/2.2f;
}

void VirusMotor::apply(Creature&creature, const MorbusExiguus&virus) {}

void VirusMotor::apply(Embryo&embryo, const MorbusExiguus&virus) {}

// Metamorphosis



// fight

void VirusMotor::fight(Immunity&immunity, Vorax&virus) {
    virus.viral_load -= immunity.value() * RandomGenerators::generate(0.10f, 0.20f);
    immunity -= immunity.value() * 0.18f;
}

void VirusMotor::fight(Immunity&immunity, MorbusExiguus&virus) {  
    virus.viral_load -= immunity.value() * 0.2f;
    immunity -= immunity.value() * 0.18f;
}
