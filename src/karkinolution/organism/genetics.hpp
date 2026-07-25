#pragma once
#include <cstdint>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/core/vec2.hpp>
#include <unordered_map>
#include <karkinolution/actions/actions.hpp>
#include <unordered_set>

/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

enum class CreatureSpecies : uint8_t {
    CRAB,
    FISH,
    CROCODILE,
    HIPPOPOTAMUS
};


namespace CreatureSpeciesF {
    CreatureSpecies choice();
}

struct MetabolismGenome {
    float max_hungry, mass, energy_limit;
    Diet diet;
};

struct ReproductionGenome {
    float reproduction_cost;
    float extra_reproduction_multiplier;
    int fertility_limit, gestation_time_limit;
};

struct BodyGenome {
    float life_limit, strength;
    int max_age;
};

struct CoreGenome {
    std::unordered_set<MoveActions> capabilities;
    Vec2 vision_radius;
    Temperament temperament;
    CreatureSpecies specie;
};

struct Genome {
    MetabolismGenome metabolism;
    ReproductionGenome reproduction;
    BodyGenome body;
    CoreGenome core;
};

namespace GenomeMotor {
    MetabolismGenome scramble(const MetabolismGenome&g1, const MetabolismGenome&g2);
    BodyGenome scramble(const BodyGenome&g1, const BodyGenome&g2);
    ReproductionGenome scramble(const ReproductionGenome&g1, const ReproductionGenome&g2);

    Genome crossover (const Genome&g1, const Genome&g2);
};

class CreatureGenomes {
    public:

    std::unordered_map<CreatureSpecies, MetabolismGenome> g_metabolism;
    std::unordered_map<CreatureSpecies, BodyGenome> g_body;
    std::unordered_map<CreatureSpecies, ReproductionGenome> g_reproduction;
    std::unordered_map<CreatureSpecies, CoreGenome> g_core;

    bool exists_specie(CreatureSpecies c_s) const;
    Genome get_genome(CreatureSpecies c_s) const;

    void add(CreatureSpecies c_s, const MetabolismGenome&g);
    void add(CreatureSpecies c_s, const BodyGenome&g);
    void add(CreatureSpecies c_s, const ReproductionGenome&g);
    void add(CreatureSpecies c_s, const CoreGenome&g);
};

namespace Populate{
    void populate_crocodile(CreatureGenomes&creature_genomes);
    void populate_fish(CreatureGenomes&creature_genomes);
    void populate_crab(CreatureGenomes&creature_genomes);
    void populate_hippopotamus(CreatureGenomes&creature_genomes);
}

inline const std::unordered_map<CreatureSpecies, float> sociability_species = {
    {CreatureSpecies::CRAB, 0.7},
    {CreatureSpecies::FISH, 0.5},
    {CreatureSpecies::CROCODILE, 0.3},
    {CreatureSpecies::HIPPOPOTAMUS, -0.67}
};

inline CreatureGenomes init () {
    CreatureGenomes cg{};

    Populate::populate_crab(cg);
    Populate::populate_fish(cg);
    Populate::populate_crocodile(cg);
    Populate::populate_hippopotamus(cg);
    return cg;
}

inline CreatureGenomes global_creature_genomes = init();
