#include "karkinolution/organism/reproduction/state/ontology.hpp"
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/organism/entities/genetics/motor.hpp>
#include <algorithm>
#include <cmath>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/entities/genetics/disturbs.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>

using GeneticDisturbs::ScrambleContext;

using CreatureMorphologyGenome = Genomes::CreatureGenomes::Morphology::MorphologyGenome;
using CreatureMetabolismGenome = Genomes::CreatureGenomes::Metabolism::MetabolismGenome;
using CreatureVitalGenome = Genomes::CreatureGenomes::Vital::VitalGenome;
using CreatureSkeletonGenome = Genomes::CreatureGenomes::SkeletonStructure::SkeletonGenome;
using CreatureMuscleGenome = Genomes::CreatureGenomes::MuscleStructure::MuscleGenome;

using EmbryoBodyGenome = Genomes::EmbryoGenomes::BodyGenome;
using EmbryoMorphologyGenome = Genomes::EmbryoGenomes::Morphology;

using ReproductionOviparous = Genomes::CreatureGenomes::Reproduction::OviparousOrganismGenome;
using ReproductionViviparous = Genomes::CreatureGenomes::Reproduction::ViviparousOrganismGenome;

using MetabolismGrowthRates = Genomes::CreatureGenomes::Metabolism::GrowthRates;
using VitalGrowthRates = Genomes::CreatureGenomes::Vital::GrowthRates;
using MuscleStructureGrowthRates = Genomes::CreatureGenomes::MuscleStructure::GrowthRates;
using SkeletonStructureGrowthRates = Genomes::CreatureGenomes::SkeletonStructure::GrowthRates;


CreatureSkeletonGenome
GenomeMotor::scramble(
    const CreatureSkeletonGenome& g1,
    const CreatureSkeletonGenome& g2
) {
    auto new_transformations = GeneticDisturbs::mix_transformations(g1.transformations, g2.transformations,
        [](
            const Efficiency& v1,
            const Efficiency& v2
        ) {
            return Efficiency{
                GeneticDisturbs::scramble(v1.value(), v2.value(), GeneticDisturbs::StandardContexts::transformations_scramble)
            };
        }
    );

    return CreatureSkeletonGenome{
        .shared_volume = GeneticDisturbs::scramble(
            g1.shared_volume.value(),
            g2.shared_volume.value(),
            GeneticDisturbs::StandardContexts::morphology_scramble
        ),
        .average_bones = GeneticDisturbs::scramble(
            g1.average_bones.value,
            g2.average_bones.value,
            GeneticDisturbs::StandardContexts::morphology_scramble
        ),
        .average_bone_quality = GeneticDisturbs::scramble(
            g1.average_bone_quality.value(),
            g2.average_bone_quality.value(),
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = GeneticDisturbs::LOW_DISTANCE_MULTIPLIER
            }
        ),
        .growth = GeneticDisturbs::scramble(
            g1.growth.value,
            g2.growth.value,
            GeneticDisturbs::StandardContexts::metabolism_growth_rates
        ),
        .transformations = new_transformations
    };
}


CreatureMorphologyGenome
GenomeMotor::scramble(
    const CreatureMorphologyGenome& g1,
    const CreatureMorphologyGenome& g2
) {
    auto new_transformations = GeneticDisturbs::mix_transformations(g1.transformations, g2.transformations,
        [](
            const Efficiency& v1,
            const Efficiency& v2
        ) {
            return Efficiency{
                GeneticDisturbs::scramble(v1.value(), v2.value(), GeneticDisturbs::StandardContexts::transformations_scramble)
            };
        }
    );

    return Genomes::CreatureGenomes::Morphology::MorphologyGenome{
        .average_lateral = GeneticDisturbs::scramble(
            g1.average_lateral.value,
            g2.average_lateral.value,
            GeneticDisturbs::StandardContexts::morphology_scramble
        ),
        .average_back = GeneticDisturbs::scramble(
            g1.average_back.value,
            g2.average_back.value,
            GeneticDisturbs::StandardContexts::morphology_scramble
        ),
        .average_height = GeneticDisturbs::scramble(
            g1.average_height.value,
            g2.average_height.value,
            ScrambleContext{
                .max_base=0.70f,
                .min_base=1.30f,
                .distance_multiplier=1.5f,
                .mutations_counts=2,
                .suppress_context=SuppressContext{
                    .chance_of_v1_being_suppressed=0.4f,
                    .chance_of_v2_being_suppressed=0.4f,
                    .suppressed_multiplier=0.35f
                }
            }
        ),
        .average_mass = GeneticDisturbs::scramble(
            g1.average_mass.value,
            g2.average_mass.value,
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = GeneticDisturbs::HIGH_DISTANCE_MULTIPLIER,
                .suppress_context = SuppressContext{
                    .chance_of_v1_being_suppressed = 0.25f,
                    .chance_of_v2_being_suppressed = 0.25f,
                    .suppressed_multiplier = 0.35f
                }
            }
        ),
        .transformations = new_transformations
    };
}

Diet GenomeMotor::scramble(const Diet& d1, const Diet& d2) {
    auto new_diet = Diet{
        // scramble: mixes two values with randomness. Parameters:
        // max_base/min_base: bounds for random multiplication factor
        // distance_multiplier: scales the difference between inputs
        GeneticDisturbs::scramble(
            d1[FoodHint::GRASS],
            d2[FoodHint::GRASS],
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = 0.75f
            }
        ),
        GeneticDisturbs::scramble(
            d1[FoodHint::RAW_MEAT],
            d2[FoodHint::RAW_MEAT],
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = 0.75f
            }
        )
    };

    return new_diet;
} 

Genomes::CreatureGenomes::Metabolism::MetabolismGenome GenomeMotor::scramble(
    const Genomes::CreatureGenomes::Metabolism::MetabolismGenome& g1,
    const Genomes::CreatureGenomes::Metabolism::MetabolismGenome& g2
) {
    auto new_transformations = GeneticDisturbs::mix_transformations(g1.transformations, g2.transformations,
        [](
            const Efficiency& v1,
            const Efficiency& v2
        ) {
            return Efficiency{
                GeneticDisturbs::scramble(v1.value(), v2.value(), GeneticDisturbs::StandardContexts::transformations_scramble)
            };
        }
    );

    auto new_growth_rates = MetabolismGrowthRates{
        .max_energy = GeneticDisturbs::scramble(
            g1.growth_rates.max_energy.value,
            g2.growth_rates.max_energy.value,
            ScrambleContext{
                .max_base = 1.15f,
                .min_base = 0.85f,
                .distance_multiplier = 0.7f,
                .inheritance_bias = 1.0f/1.9f,
                .mutations_counts = 2
            }
        ),
        .energy = GeneticDisturbs::scramble(
            g1.growth_rates.energy.value,
            g2.growth_rates.energy.value,
            ScrambleContext{
                .max_base = 1.15f,
                .min_base = 0.85f,
                .distance_multiplier = GeneticDisturbs::MEDIUM_DISTANCE_MULTIPLIER,
                .inheritance_bias = 1.0f/2.1f
            }
        ),
        .max_reserved_energy = GeneticDisturbs::scramble(
            g1.growth_rates.max_reserved_energy.value,
            g2.growth_rates.max_reserved_energy.value,
            GeneticDisturbs::StandardContexts::metabolism_growth_rates
        )
    };

    return Genomes::CreatureGenomes::Metabolism::MetabolismGenome{
        .max_energy = GeneticDisturbs::scramble(
            g1.max_energy,
            g2.max_energy,
            ScrambleContext{
                .max_base = 0.80f,
                .min_base = 1.20f,
                .distance_multiplier = 0.2f
            }
        ),
        .max_reserved_energy = GeneticDisturbs::scramble(
            g1.max_reserved_energy,
            g2.max_reserved_energy,
            ScrambleContext{
                .max_base = 0.91f,
                .min_base = 1.09f,
                .distance_multiplier = 0.1f
            }
        ),
        .diet = scramble(g1.diet, g2.diet),
        .transformations = new_transformations,
        .growth_rates = new_growth_rates
    };
}
ReproductionViviparous GenomeMotor::scramble(const ReproductionViviparous&g1, const ReproductionViviparous&g2) {
    return {};
}

Genomes::CreatureGenomes::Reproduction::OviparousOrganismGenome 
GenomeMotor::scramble(
    const Genomes::CreatureGenomes::Reproduction::OviparousOrganismGenome& g1,
    const Genomes::CreatureGenomes::Reproduction::OviparousOrganismGenome& g2
) {
    return {};
}

ReproductiveGenome GenomeMotor::scramble(
    const ReproductiveGenome& g1,
    const ReproductiveGenome& g2
) {
    const auto children = std::max(1, static_cast<int>(std::lround(GeneticDisturbs::scramble(
        g1.average_children_count,
        g2.average_children_count,
        ScrambleContext{
            .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
            .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
            .distance_multiplier = GeneticDisturbs::LOW_DISTANCE_MULTIPLIER
        }
    ))));

    const int fertility_limit = std::max(1, static_cast<int>(std::lround(GeneticDisturbs::scramble(
            g1.fertility_limit,
            g2.fertility_limit,
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = GeneticDisturbs::LOW_DISTANCE_MULTIPLIER
            }
        ))));

    const float reproduction_cost = GeneticDisturbs::scramble(
            g1.reproduction_cost.value(),
            g2.reproduction_cost.value(),
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = GeneticDisturbs::LOW_DISTANCE_MULTIPLIER
            }
        );
    const int average_gestation_limit = std::max(1, static_cast<int>(std::lround(GeneticDisturbs::scramble(
                g1.average_gestation_limit,
                g2.average_gestation_limit,
                ScrambleContext{
                    .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                    .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                    .distance_multiplier = GeneticDisturbs::LOW_DISTANCE_MULTIPLIER
                }
            ))));


    if (g1.reproductive_way() != g2.reproductive_way()) {
        const auto chosen = Choices::choice({g1, g2});
        
        return ReproductiveGenome{
            .reproductive_way_genome = std::move(chosen.reproductive_way_genome),
            .average_children_count = children,
            .average_gestation_limit = average_gestation_limit,
            .fertility_limit = fertility_limit,
            .reproduction_cost = reproduction_cost,

    };
    }

    std::variant<ReproductionViviparous, ReproductionOviparous> new_way_genome;
    if (g1.reproductive_way() == ReproductiveWays::OVIPAROUS) {
        new_way_genome = GenomeMotor::scramble(
            std::get<ReproductionOviparous>(g1.reproductive_way_genome),
            std::get<ReproductionOviparous>(g1.reproductive_way_genome)
        );
    } else { // reproductive_way == VIVIPAROUS
        new_way_genome = GenomeMotor::scramble(
            std::get<ReproductionViviparous>(g1.reproductive_way_genome),
            std::get<ReproductionViviparous>(g2.reproductive_way_genome)
        );
    }

    return {
        .reproductive_way_genome = new_way_genome,
        .average_children_count = children,
        .average_gestation_limit = average_gestation_limit,
        .fertility_limit = fertility_limit,
        .reproduction_cost = reproduction_cost
    };
}

Genomes::CreatureGenomes::Vital::VitalGenome 
GenomeMotor::scramble(
    const Genomes::CreatureGenomes::Vital::VitalGenome& g1,
    const Genomes::CreatureGenomes::Vital::VitalGenome& g2
) {
    auto new_transformations = GeneticDisturbs::mix_transformations(g1.transformations, g2.transformations,
        [](
            const Efficiency& v1,
            const Efficiency& v2
        ) {
            return Efficiency{
                GeneticDisturbs::scramble(v1.value(), v2.value(), GeneticDisturbs::StandardContexts::transformations_scramble)
            };
        }
    );

    auto new_growth_rates = VitalGrowthRates{
        .life = GeneticDisturbs::scramble(
            g1.growth_rates.life.value,
            g2.growth_rates.life.value,
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = GeneticDisturbs::MEDIUM_DISTANCE_MULTIPLIER
            }
        )
    };

    return CreatureVitalGenome{
        .max_life = GeneticDisturbs::scramble(
            g1.max_life,
            g2.max_life,
            ScrambleContext{
                .max_base = 1.10f,
                .min_base = 0.90f,
                .distance_multiplier = GeneticDisturbs::MEDIUM_DISTANCE_MULTIPLIER
            }
        ),
        .average_immunity = GeneticDisturbs::scramble(
            g1.average_immunity.value(),
            g2.average_immunity.value(),
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = GeneticDisturbs::LOW_DISTANCE_MULTIPLIER
            }
        ),
        .average_health = GeneticDisturbs::scramble(
            g1.average_health.value(),
            g2.average_health.value(),
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = GeneticDisturbs::LOW_DISTANCE_MULTIPLIER
            }
        ),
        .average_age = GeneticDisturbs::scramble(
            g1.average_age.value,
            g2.average_age.value,
            ScrambleContext{
                .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
                .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
                .distance_multiplier = GeneticDisturbs::HIGH_DISTANCE_MULTIPLIER
            }
        ),
        .transformations = new_transformations,
        .growth_rates = new_growth_rates
    };
}

BrainGenome 
GenomeMotor::scramble(
    const BrainGenome& g1,
    const BrainGenome& g2
) {
    return BrainGenome{
        .average_sociability = GeneticDisturbs::scramble(
            g1.average_sociability.value(),
            g2.average_sociability.value(),
            ScrambleContext{
                .max_base = 1.30f,
                .min_base = 0.70f,
                .distance_multiplier = 1.0f,
                .mutations_counts = 3
            }
        )
    };
}

Genomes::EmbryoGenomes::BodyGenome 
GenomeMotor::scramble(
    const Genomes::EmbryoGenomes::BodyGenome& g1,
    const Genomes::EmbryoGenomes::BodyGenome& g2
) {
    // Embryonic reserves are strongly constrained by development.  Keep the
    // mutation window small while allowing the parental difference to matter.
    const ScrambleContext body_context{
        .max_base = GeneticDisturbs::LOW_SCRAMBLE_MAX_BASE,
        .min_base = GeneticDisturbs::LOW_SCRAMBLE_MIN_BASE,
        .distance_multiplier = GeneticDisturbs::LOW_DISTANCE_MULTIPLIER,
        .mutations_counts = 1,
        .suppress_context = SuppressContext{
            .chance_of_v1_being_suppressed = 0.20f,
            .chance_of_v2_being_suppressed = 0.20f,
            .suppressed_multiplier = 0.40f
        }
    };

    return EmbryoBodyGenome{
        .average_max_energy = GeneticDisturbs::scramble(
            g1.average_max_energy.value(),
            g2.average_max_energy.value(),
            body_context
        ),
        .average_max_life = GeneticDisturbs::scramble(
            g1.average_max_life.value(),
            g2.average_max_life.value(),
            body_context
        ),
        .average_health = GeneticDisturbs::scramble(
            g1.average_health.value(),
            g2.average_health.value(),
            body_context
        )
    };
}

Genomes::EmbryoGenomes::Morphology 
GenomeMotor::scramble(
    const Genomes::EmbryoGenomes::Morphology& g1,
    const Genomes::EmbryoGenomes::Morphology& g2
) {
    // Growth rate is more plastic during embryogenesis than the final size;
    // final volume therefore uses the same restrained morphology context used
    // by the adult genome.
    return EmbryoMorphologyGenome{
        .volume_growth = GeneticDisturbs::scramble(
            g1.volume_growth,
            g2.volume_growth,
            ScrambleContext{
                .max_base = 1.15f,
                .min_base = 0.85f,
                .distance_multiplier = GeneticDisturbs::MEDIUM_DISTANCE_MULTIPLIER,
                .mutations_counts = 1,
                .suppress_context = SuppressContext{
                    .chance_of_v1_being_suppressed = 0.18f,
                    .chance_of_v2_being_suppressed = 0.18f,
                    .suppressed_multiplier = 0.50f
                }
            }
        ),
        .average_volume = GeneticDisturbs::scramble(
            g1.average_volume.value(),
            g2.average_volume.value(),
            GeneticDisturbs::StandardContexts::morphology_scramble
        )
    };
}

Genome 
GenomeMotor::crossover(
    const Genome& g1,
    const Genome& g2
) {
    return Genome{
        .creature_genome = crossover(g1.creature_genome, g2.creature_genome),
        .embryo_genome = crossover(g1.embryo_genome, g2.embryo_genome),
        .core_genome = crossover(g1.core_genome, g2.core_genome),
        .brain_genome = scramble(g1.brain_genome, g2.brain_genome),
    };
}
