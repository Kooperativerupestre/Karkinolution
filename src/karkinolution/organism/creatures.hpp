#pragma once
#include "karkinolution/core/basestorage.hpp"
#include "karkinolution/organism/ontology.hpp"
#include <karkinolution/core/stats.hpp>
#include <karkinolution/organism/genetics.hpp>
#include <cstdint>
#include <string>
#include <optional>
#include <variant>
#include <karkinolution/core/error.hpp>

inline constexpr float MAX_STRENGTH = 35.0f;

/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

class Gestation : public IntegerLimited<uint16_t> {
    public:

    using IntegerLimited<uint16_t>::IntegerLimited;

    bool is_overdue() const;
    bool is_ready_to_born() const;
    bool is_premature() const;
    bool is_at_point() const;
    NormalizedValue death_factor() const;
    uint16_t overdue() const;
    uint16_t time_to_born() const;
};

using ChildrenCount = IntegerLimited<uint8_t>;

struct PregnantUterus {
    Genome male_genome;
    Gestation gestation;
    ChildrenCount children_count;


    bool all_children_borned() const;
    float pregnancy_cost() const;
    NormalizedValue gravity() const;
    float birth_energy() const;
};

struct EmptyUterus{};

using Uterus = std::variant<std::monostate, PregnantUterus, EmptyUterus>;

struct Creature {
    Genome genome;
    Gender gender;
    std::string name;
    uint64_t id;

    Energy energy;
    Life life;
    Age age;
    Fertility fertility;
    Intent intent = Intent{IntentActs::NOTHING};
    std::optional<AttackedEvent> last_attack = std::nullopt;
    Uterus uterus;
    NormalizedValue sociability;

    Vec2 position;

    Creature(const Creature&) = delete;
    Creature& operator=(const Creature&) = delete;

    Creature(Creature&&) = default;
    Creature& operator=(Creature&&) = default;

    Creature(
        Genome genome,
        Gender gender,
        std::string name,
        uint64_t id,
        Energy energy,
        Life life,
        Age age,
        Fertility fertility,
        Uterus uterus,
        NormalizedValue sociability,
        Vec2 position
    )
        : genome(std::move(genome)),
          gender(gender),
          name(std::move(name)),
          id(id),
          energy(energy),
          life(life),
          age(age),
          fertility(fertility),
          uterus(std::move(uterus)),
          sociability(sociability),
          position(position)
    {}
    NormalizedValue hungry() const;
    float needed_energy() const;
    NormalizedValue reproductive_maturity() const;
    bool reproductively_capable() const;
    bool pregnant() const;
    NormalizedValue reproductive_fitness() const;
    NormalizedValue strength_factor() const;
    NormalizedValue physical_ratio() const;
    NormalizedValue senescence() const;
    float basal_metabolism() const;
};
struct Corpse {
    Energy energy;
    uint64_t id;
    IntegerLimited<uint16_t> decomposition_time;
    Vec2 position;

    Corpse(
        Energy energy,
        uint64_t id,
        IntegerLimited<uint16_t> decomposition_time,
        Vec2 position
    )
        : energy(std::move(energy)),
          id(id),
          decomposition_time(decomposition_time),
          position(position)
    {}


    Corpse(const Corpse&) = delete;
    Corpse& operator=(const Corpse&) = delete;

    Corpse(Corpse&&) = default;
    Corpse& operator=(Corpse&&) = default;

    int time_left() const;
    bool ready_to_disapear() const;
};

using Entity = std::variant<Creature, Corpse>;

class EntitiesRegistry : public BaseStorage<Id, Entity> {
    public:

    EntitiesRegistry() : BaseStorage<Id, std::variant<Creature, Corpse>>() {}

    Creature& at_creature(Id id);
    const Creature& at_creature(Id id) const;

    Corpse& at_corpse(Id id);
    const Corpse& at_corpse(Id id) const;
};

namespace EntityGetters {
    Id get_id(const Entity& entity);
    Id get_id(const Corpse& corpse);
    Id get_id(const Creature& creature);

    Vec2 get_position(const Entity& entity);
    Vec2 get_position(const Corpse& corpse);
    Vec2 get_position(const Creature& creature);
}

struct CreatureConfig {
    std::optional<Vec2> position;
    std::optional<CreatureSpecies> specie;
    std::optional<Id> id;
    std::optional<Energy> initial_energy;
    std::optional<Genome> genome;
    std::optional<Gender> gender;
    std::optional<std::string> name;
    std::optional<NormalizedValue> sociability;
};

namespace CreatureFactory {
    Creature gen_creature(const CreatureConfig&config);
}