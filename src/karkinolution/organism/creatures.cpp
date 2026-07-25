
#include <karkinolution/organism/ontology.hpp>
#include <cassert>
#include <karkinolution/organism/genetics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/core/stats.hpp>
#include <variant>
#include <karkinolution/utils/namegenerator.hpp>
#include <karkinolution/utils/k_random.hpp>


// Gestation

bool Gestation::is_overdue() const {
    return value() > max() + 1;
}

bool Gestation::is_ready_to_born() const {
    return value() >= max();
}

bool Gestation::is_premature() const {
    return value() + 1 < max();
}

bool Gestation::is_at_point() const {
    return value() == max();
}

NormalizedValue Gestation::death_factor() const {
    NormalizedValue factor = {0};

    if (is_premature()) {
        uint16_t time_born = time_to_born();
        factor += static_cast<float>(time_born)/(time_born + max())*1.40f;
    } else if (is_overdue()) {
        uint16_t overd = overdue();
        factor += static_cast<float>(overd)/(overd + max());
    }

    return factor;
}


uint16_t Gestation::overdue() const {
    return std::max<uint16_t>(0, (value() - max()) ? value() > max() : 0);
}

uint16_t Gestation::time_to_born() const {
    return std::max<uint16_t>(0, (max() - value()) ? max() > value() : 0);
}

// PregnantUterus

bool PregnantUterus::all_children_borned() const {
    return children_count.value() == children_count.max();
}

float PregnantUterus::pregnancy_cost() const {
    return (1 + static_cast<float>(gestation.value())/2)*children_count.value();
}

NormalizedValue PregnantUterus::gravity() const {
    return std::pow(((children_count.ratio().value() + gestation.ratio().value())/2), 1.4);
}

float PregnantUterus::birth_energy() const {
    return pregnancy_cost()/children_count.value();
}

// Creature

NormalizedValue Creature::hungry() const {
    return NormalizedValue{1.0f - energy.ratio().value()};
}

float Creature::needed_energy() const {
    return energy.max() - energy.value();
}

NormalizedValue Creature::reproductive_maturity() const {
    return static_cast<float>(std::exp(std::pow(std::pow(-(-age.ratio().value() - 0.45)
                            , 2)/(0.2), 2)));
}

bool Creature::pregnant() const {
    return std::holds_alternative<PregnantUterus>(uterus);
}

bool Creature::reproductively_capable() const {
    bool energy_condition = energy.value() * genome.reproduction.extra_reproduction_multiplier > genome.reproduction.reproduction_cost;
    return fertility.reproductive_capability() && energy_condition && ! pregnant();
}

NormalizedValue Creature::reproductive_fitness() const {
    return NormalizedValue{energy.value() * genome.reproduction.extra_reproduction_multiplier / genome.reproduction.reproduction_cost};
}

NormalizedValue Creature::strength_factor() const {
    return NormalizedValue{genome.body.strength/MAX_STRENGTH};
}

NormalizedValue Creature::physical_ratio() const {
    return NormalizedValue{static_cast<float>((energy.ratio().value()*1.2 + life.ratio().value()*1.5 + strength_factor().value()*0.9)/3.6)};
}

NormalizedValue Creature::senescence() const {
    return NormalizedValue{static_cast<float>(age.value()/(std::pow(age.max(), 1.4)))};
}

float Creature::basal_metabolism() const {
    return genome.metabolism.mass;
}

// Corpse

int Corpse::time_left() const {
    return decomposition_time.max() - decomposition_time.value() ? decomposition_time.max() > decomposition_time.value() : 0;
}

bool Corpse::ready_to_disapear() const {
    return decomposition_time.value()==decomposition_time.max();
}

// EntitiesRegistry

Creature& EntitiesRegistry::at_creature(Id id) {
    assert(id.entity_type == EntityTypes::CREATURE);
    return std::get<Creature>(at(id));
}

const Creature& EntitiesRegistry::at_creature(Id id) const {
    assert(id.entity_type == EntityTypes::CREATURE);
    return std::get<Creature>(at(id));
}

Corpse& EntitiesRegistry::at_corpse(Id id) {
    assert(id.entity_type == EntityTypes::CORPSE);
    return std::get<Corpse>(at(id));
}

const Corpse& EntitiesRegistry::at_corpse(Id id) const {
    assert(id.entity_type == EntityTypes::CORPSE);
    return std::get<Corpse>(at(id));
}

Id EntityGetters::get_id(const Entity& entity) {
    return std::visit([](const auto& entity) {
        return EntityGetters::get_id(entity);
    }, entity);
}

Id EntityGetters::get_id(const Corpse& corpse) {
    return IDF::create_corpse_id(corpse.id);
}

Id EntityGetters::get_id(const Creature& creature) {
    return IDF::create_creature_id(creature.id);
}


Vec2 EntityGetters::get_position(const Entity& entity) {
    return std::visit([](const auto& entity) {
        return EntityGetters::get_position(entity);
    }, entity);
}

Vec2 EntityGetters::get_position(const Corpse& corpse) {
    return corpse.position;
}

Vec2 EntityGetters::get_position(const Creature& creature) {
    return creature.position;
}
// CreatureFactory

Creature CreatureFactory::gen_creature(const CreatureConfig& config)
{
    auto position = config.position.value_or(Vec2(0, 0));

    auto specie = config.specie.value_or(
        CreatureSpeciesF::choice()
    );

    auto genome = config.genome.value_or(
        global_creature_genomes.get_genome(specie)
    );

    auto id = config.id.value_or(
        IDF::create_creature_id(IDF::gen_id())
    );

    auto initial_energy = config.initial_energy;

    if (!initial_energy.has_value()) {
        float initial_energy_value =
            Disturbs::gen_disturb(0.5, 1) * genome.metabolism.energy_limit;

        initial_energy = Energy(
            initial_energy_value,
            genome.metabolism.energy_limit
        );
    }

    auto gender = config.gender.value_or(
        GenderF::choice()
    );

    auto name = config.name.value_or(
        gen_name()
    );

    auto sociability = config.sociability.value_or(
        sociability_species.at(specie)
    );

    Uterus uterus{std::monostate{}};

    if (gender == Gender::FEMALE) {
        uterus = EmptyUterus{};
    }

    return Creature{
        genome,
        gender,
        std::move(name),
        id.value,
        *initial_energy,
        Life(
            genome.body.life_limit,
            genome.body.life_limit
        ),
        Age{
            0,
            static_cast<uint16_t>(genome.body.max_age)
        },
        Fertility{
            0,
            genome.reproduction.fertility_limit
        },
        std::move(uterus),
        sociability,
        position
    };
}