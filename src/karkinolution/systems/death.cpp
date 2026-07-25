#include <karkinolution/organism/genetics.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/systems/death.hpp>
#include <cmath>

Corpse DeathSystem::generate_corpse(const Creature&creature) {
    float energy = creature.energy.value() * 0.8;
    if (creature.pregnant()) {
        energy += std::get<PregnantUterus>(creature.uterus).pregnancy_cost() * 2;
    }
    IntegerLimited<uint16_t> decomposition_time = {std::round(energy/creature.energy.max() * 7.5)};
    return Corpse{
        Energy(energy, energy),
        IDF::gen_id(),
        decomposition_time,
        creature.position
    };
}

bool DeathSystem::is_dead(const Creature &creature) {
    return creature.life.value() < 1e-9 || creature.energy.value() < 1e-9 || creature.age.value() >= creature.age.max();
}
