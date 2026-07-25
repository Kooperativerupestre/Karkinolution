#include <karkinolution/organism/genetics.hpp>
#include <karkinolution/utils/k_random.hpp>

// CreatureSpeciesF

CreatureSpecies CreatureSpeciesF::choice() {
    return Choices::choice({
        CreatureSpecies::CRAB, 
        CreatureSpecies::CROCODILE,
        CreatureSpecies::FISH, 
        CreatureSpecies::HIPPOPOTAMUS
    });
}

// GenomeMotor
MetabolismGenome GenomeMotor::scramble(const MetabolismGenome& g1, const MetabolismGenome& g2) {
    return MetabolismGenome{
        .max_hungry = Disturbs::smooth_scramble(g1.max_hungry, g2.max_hungry),
        .mass = Disturbs::gen_little_disturb() * g1.mass,
        .energy_limit = (g1.energy_limit + g2.energy_limit) / 2 * Disturbs::gen_medium_disturb(),
        .diet = g1.diet.scramble(g2.diet)
    };
}

ReproductionGenome GenomeMotor::scramble(const ReproductionGenome& g1, const ReproductionGenome& g2) {
    return ReproductionGenome{
        .reproduction_cost = Disturbs::smooth_scramble(g1.reproduction_cost, g2.reproduction_cost),
        .extra_reproduction_multiplier = Disturbs::smooth_scramble(g1.extra_reproduction_multiplier, g2.extra_reproduction_multiplier),
        .fertility_limit = Disturbs::time_scramble(g1.fertility_limit, g2.fertility_limit),
        .gestation_time_limit = Disturbs::time_scramble(g1.gestation_time_limit, g2.gestation_time_limit)
    };
}

BodyGenome GenomeMotor::scramble(const BodyGenome& g1, const BodyGenome& g2) {
    return BodyGenome{
        .life_limit = Disturbs::smooth_scramble(g1.life_limit, g2.life_limit),
        .strength = Disturbs::smooth_scramble(g1.strength, g2.strength),
        .max_age = Disturbs::time_scramble(g1.max_age, g2.max_age)
    };
}

Genome GenomeMotor::crossover(const Genome& g1, const Genome& g2) {
    return Genome{
        .metabolism = scramble(g1.metabolism, g2.metabolism),
        .reproduction = scramble(g1.reproduction, g2.reproduction),
        .body = scramble(g1.body, g2.body),
        .core = g1.core
    };
}

// CreatureGenoms

bool CreatureGenomes::exists_specie(CreatureSpecies c_s) const {
    return g_metabolism.contains(c_s) &&
           g_body.contains(c_s) &&
           g_reproduction.contains(c_s) &&
           g_core.contains(c_s);
}

Genome CreatureGenomes::get_genome(CreatureSpecies c_s) const {
    return Genome{
        .metabolism = g_metabolism.at(c_s),
        .reproduction = g_reproduction.at(c_s),
        .body = g_body.at(c_s),
        .core = g_core.at(c_s)
    };
}

void CreatureGenomes::add(CreatureSpecies c_s, const MetabolismGenome& g) {
    g_metabolism.insert_or_assign(c_s, g);
}

void CreatureGenomes::add(CreatureSpecies c_s, const BodyGenome& g) {
    g_body.insert_or_assign(c_s, g);
}

void CreatureGenomes::add(CreatureSpecies c_s, const ReproductionGenome& g) {
    g_reproduction.insert_or_assign(c_s, g);
}

void CreatureGenomes::add(CreatureSpecies c_s, const CoreGenome& g) {
    g_core.insert_or_assign(c_s, g);
}
// Populate

// ==========================================
// Populate Namespace
// ==========================================

void Populate::populate_crocodile(CreatureGenomes& creature_genomes) {
    creature_genomes.add(CreatureSpecies::CROCODILE, MetabolismGenome{
        .max_hungry = 0.80f,
        .mass = 1.25f,
        .energy_limit = 100.0f,
        .diet = Diet(0.58f, 0.085f, 0.85f) // Assumindo construtor Diet(float, float, float)
    });

    creature_genomes.add(CreatureSpecies::CROCODILE, BodyGenome{
        .life_limit = 100.0f,
        .strength = 27.0f,
        .max_age = 50
    });

    creature_genomes.add(CreatureSpecies::CROCODILE, ReproductionGenome{
        .reproduction_cost = 40.0f,
        .extra_reproduction_multiplier = 1.2f,
        .fertility_limit = 5,
        .gestation_time_limit = 5
    });

    creature_genomes.add(CreatureSpecies::CROCODILE, CoreGenome{
        .capabilities = {MoveActions::WALK, MoveActions::SWIMM},
        .vision_radius = Vec2(4, 4),
        .temperament = Temperament::AGGRESSIVE,
        .specie = CreatureSpecies::CROCODILE
    });
}

void Populate::populate_fish(CreatureGenomes& creature_genomes) {
    creature_genomes.add(CreatureSpecies::FISH, MetabolismGenome{
        .max_hungry = 0.33f,
        .mass = 1.0f,
        .energy_limit = 10.0f,
        .diet = Diet(0.2f, 0.75f, 0.15f)
    });

    creature_genomes.add(CreatureSpecies::FISH, BodyGenome{
        .life_limit = 9.0f,
        .strength = 1.0f,
        .max_age = 10
    });

    creature_genomes.add(CreatureSpecies::FISH, ReproductionGenome{
        .reproduction_cost = 1.9f,
        .extra_reproduction_multiplier = 1.0f,
        .fertility_limit = 1,
        .gestation_time_limit = 2
    });

    creature_genomes.add(CreatureSpecies::FISH, CoreGenome{
        .capabilities = {MoveActions::SWIMM},
        .vision_radius = Vec2(2, 4),
        .temperament = Temperament::PASSIVE,
        .specie = CreatureSpecies::FISH
    });
}

void Populate::populate_crab(CreatureGenomes& creature_genomes) {
    creature_genomes.add(CreatureSpecies::CRAB, MetabolismGenome{
        .max_hungry = 0.65f,
        .mass = 1.1f,
        .energy_limit = 40.0f,
        .diet = Diet(0.5f, 0.35f, 0.15f)
    });

    creature_genomes.add(CreatureSpecies::CRAB, BodyGenome{
        .life_limit = 30.0f,
        .strength = 7.5f,
        .max_age = 30
    });

    creature_genomes.add(CreatureSpecies::CRAB, ReproductionGenome{
        .reproduction_cost = 5.0f,
        .extra_reproduction_multiplier = 1.2f,
        .fertility_limit = 3,
        .gestation_time_limit = 4
    });

    creature_genomes.add(CreatureSpecies::CRAB, CoreGenome{
        .capabilities = {MoveActions::WALK},
        .vision_radius = Vec2(3, 3),
        .temperament = Temperament::NEUTRAL,
        .specie = CreatureSpecies::CRAB
    });
}

void Populate::populate_hippopotamus(CreatureGenomes& creature_genomes) {
    creature_genomes.add(CreatureSpecies::HIPPOPOTAMUS, MetabolismGenome{
        .max_hungry = 0.70f,
        .mass = 1.6f,
        .energy_limit = 85.0f,
        .diet = Diet(0.2f, 0.7f, 0.1f)
    });

    creature_genomes.add(CreatureSpecies::HIPPOPOTAMUS, BodyGenome{
        .life_limit = 150.0f,
        .strength = 20.0f,
        .max_age = 60
    });

    creature_genomes.add(CreatureSpecies::HIPPOPOTAMUS, ReproductionGenome{
        .reproduction_cost = 40.0f,
        .extra_reproduction_multiplier = 1.3f,
        .fertility_limit = 4,
        .gestation_time_limit = 5
    });

    creature_genomes.add(CreatureSpecies::HIPPOPOTAMUS, CoreGenome{
        .capabilities = {MoveActions::WALK, MoveActions::SWIMM},
        .vision_radius = Vec2(3, 3),
        .temperament = Temperament::TERRITORIAL,
        .specie = CreatureSpecies::HIPPOPOTAMUS
    });
}

