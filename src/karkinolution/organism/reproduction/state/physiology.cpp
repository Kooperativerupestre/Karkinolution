#include "karkinolution/core/id_generator.hpp"
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/genetics/motor.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"
#include <karkinolution/organism/reproduction/state/physiology.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <random>


int ReproductionOrganPhysiology::get_children_count(Creature &creature) {
    std::poisson_distribution<int> distribution(creature.genome.creature_genome.reproductive.average_children_count);

    return distribution(gen);
    // Poisson distribution is used here because repeated samples converge to the specified mean.
}

float ReproductionOrganPhysiology::needed_energy_of_all_embryos(const ReproductionOrgan &organ, const OrganismRegistry &organisms) {
    ReproductionValidator::is_pregnant(organ);

    float total_energy = 0.0f;
    const auto& uterus = std::get<Uterus>(organ.state);

    for (auto id : uterus.get_pregnant_uterus().embryos) {
        const auto& embryo = organisms.entities.at_embryo(IDF::create_embryo_id(id));
        total_energy += (embryo.energy.max() - embryo.energy.value());
    }
    return total_energy;
}
