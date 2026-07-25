#pragma once
#include <karkinolution/organism/genetics.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/brain/perception.hpp>



struct BornData {
    Genome genome;
    Energy initial_energy;
    float sociability;
};

struct ReproductionCost {
    const float female_cost, male_cost;
};

struct Parents {
    const Id female_id, male_id;
};

namespace UterusSystem {
    void finish(Creature&creature);
    int random_children_number();
    bool die_a_child(float death_tax);
    void conceive(Creature&creature, const Genome&genome);
    std::optional<BornData> have_child(Creature&creature);
    void pass_time(Creature&creature);
}

namespace ReproductiveSystem {
    std::optional<BornData> to_birth(Creature&female);
    bool can_reproduce(const Creature&A, const PerceivedCreature&B);
    Parents return_parents(const Creature&A, const Creature&B);
    std::optional<ReproductionCost> reproduce(Creature&creature, Creature&male);
}