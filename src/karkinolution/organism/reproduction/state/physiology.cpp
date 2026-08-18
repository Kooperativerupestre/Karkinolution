#include "karkinolution/organism/reproduction/state/state.hpp"
#include <karkinolution/organism/reproduction/state/physiology.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <random>


int ReproductionOrganPhysiology::get_children_count(Creature &creature) {
    std::poisson_distribution<int> distribution(creature.genome.creature_genome.reproductive.average_children_count);

    return distribution(gen);
    // Poisson distribution is used here because repeated samples converge to the specified mean.
}

