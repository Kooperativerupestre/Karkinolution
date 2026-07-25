#include "karkinolution/organism/genetics.hpp"
#include <karkinolution/organism/ontology.hpp>
#include <cassert>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/core/stats.hpp>
#include <optional>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/systems/reproduction.hpp>
#include <karkinolution/organism/validator.hpp>

// ==========================================
// UterusSystem
// ==========================================

void UterusSystem::finish(Creature& creature) {
    Validator::validate_pregnant(creature);
    creature.uterus = EmptyUterus();
}

int UterusSystem::random_children_number() {
    std::vector<int> children_count{1, 2, 3};
    std::vector<float> weights{1.0f / 2.0f, 1.0f / 4.0f, 1.0f / 8.0f};

    return Choices::choices(children_count, weights, 1)[0];
}

bool UterusSystem::die_a_child(float death_tax) {
    return Choices::choice_bool(death_tax, 1.0f - death_tax);
}

void UterusSystem::conceive(Creature& creature, const Genome& genome) {
    Validator::validate_non_pregnant(creature);
    Validator::validate_same_species(creature.genome.core.specie, genome.core.specie);

    ChildrenCount children_cout{UterusSystem::random_children_number()};

    Gestation gestation{creature.genome.reproduction.gestation_time_limit};
    creature.uterus = PregnantUterus(genome, gestation, children_cout);
}

std::optional<BornData> UterusSystem::have_child(Creature& creature) {
    Validator::validate_pregnant(creature);
    PregnantUterus* uterus = &std::get<PregnantUterus>(creature.uterus);
    if (UterusSystem::die_a_child(uterus->gestation.death_factor().value())) {
        return std::nullopt;
    }
    
    uterus->children_count.pass();

    Genome child_genome = GenomeMotor::crossover(creature.genome, uterus->male_genome);
    float sociability = creature.sociability.value() * Disturbs::gen_disturb(0.7, 1.3);
    float initial_energy = uterus->birth_energy();
    BornData child{
        child_genome,
        Energy(initial_energy, initial_energy),
        sociability
    };

    if (uterus->all_children_borned()) {
        UterusSystem::finish(creature);
    }
    return child;
}

void UterusSystem::pass_time(Creature& creature) {
    Validator::validate_female_gender(creature);
    PregnantUterus* uterus = &std::get<PregnantUterus>(creature.uterus);
    if (creature.pregnant()) {
        if (uterus->all_children_borned()) {
            UterusSystem::finish(creature);
        }
        uterus->gestation.pass();
    }
}

// ReproductiveSystems

std::optional<BornData> ReproductiveSystem::to_birth(Creature& female) {
    Validator::validate_pregnant(female);

    auto child = UterusSystem::have_child(female);

    female.fertility.zero();

    return child;
}

bool ReproductiveSystem::can_reproduce(const Creature& A, const PerceivedCreature& B) {
    return A.reproductively_capable() && A.gender == GenderF::other_sex(B.ontology.gender) && B.body.reproductive_capacity && B.ontology.specie == A.genome.core.specie;
}

Parents ReproductiveSystem::return_parents(const Creature& A, const Creature& B) {
    Gender a_gender = A.gender;
    Gender b_gender = B.gender;

    Validator::validate_different_genders(A.gender, B.gender);

    Id p_female_id = IDF::create_creature_id(a_gender == Gender::FEMALE ? a_gender == Gender::FEMALE : B.id);
    Id p_male_id = IDF::create_creature_id(b_gender == Gender::MALE ? B.id : A.id);
    return Parents{p_female_id, p_male_id}; 
}

std::optional<ReproductionCost> ReproductiveSystem::reproduce(Creature &female, Creature &male) {
    if (female.energy.value() < Approx<float>(female.genome.reproduction.reproduction_cost)
            || male.energy.value() < Approx<float>(female.genome.reproduction.reproduction_cost)) {
        return std::nullopt;
            }
    Validator::check_energy(female, female.genome.reproduction.reproduction_cost);
    Validator::check_energy(male, male.genome.reproduction.reproduction_cost);

    Validator::validate_female_gender(female);
    Validator::validate_male_gender(male);

    Validator::validate_reproductive_capacity(female);
    Validator::validate_reproductive_capacity(male);

    UterusSystem::conceive(female, male.genome);
    female.fertility.zero();
    male.fertility.zero();

    return ReproductionCost{
        .female_cost = female.genome.reproduction.reproduction_cost,
        .male_cost = male.genome.reproduction.reproduction_cost
    };
}