#pragma once
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>

using CreatureMorphologyGenome = Genomes::CreatureGenomes::Morphology::MorphologyGenome;
using CreatureMetabolismGenome = Genomes::CreatureGenomes::Metabolism::MetabolismGenome;
using CreatureVitalGenome = Genomes::CreatureGenomes::Vital::VitalGenome;
using CreatureSkeletonGenome = Genomes::CreatureGenomes::SkeletonStructure::SkeletonGenome;
using CreatureMuscleGenome = Genomes::CreatureGenomes::MuscleStructure::MuscleGenome;

using EmbryoBodyGenome = Genomes::EmbryoGenomes::BodyGenome;
using EmbryoMorphologyGenome = Genomes::EmbryoGenomes::Morphology;

using ReproductionOviparous = Genomes::CreatureGenomes::Reproduction::OviparousOrganismGenome;
using ReproductionViviparous = Genomes::CreatureGenomes::Reproduction::ViviparousOrganismGenome;
using ReproductiveGenome = Genomes::CreatureGenomes::Reproduction::ReproductiveGenome;

namespace GenomeMotor {

    Diet scramble(const Diet&d1, const Diet&d2);

    CreatureMorphologyGenome scramble(
        const CreatureMorphologyGenome& g1,
        const CreatureMorphologyGenome& g2
    );

    CreatureSkeletonGenome scramble(
        const CreatureSkeletonGenome& g1,
        const CreatureSkeletonGenome& g2
    );

    CreatureMuscleGenome scramble(
        const CreatureMuscleGenome& g1,
        const CreatureMuscleGenome& g2
    );
    

    CreatureVitalGenome scramble(
        const CreatureVitalGenome& g1,
        const CreatureVitalGenome& g2        
    );
    

    CreatureMetabolismGenome scramble(
        const CreatureMetabolismGenome& g1,
        const CreatureMetabolismGenome& g2
    );

    ReproductionOviparous scramble(
        const ReproductionOviparous& g1,
        const ReproductionOviparous& g2
    );
    

    ReproductionViviparous scramble(
        const ReproductionViviparous& g1,
        const ReproductionViviparous& g2
    );


    /*
    scramble(ReproductiveGenome)

    If the reproductive ways are different, one must be chosen,
    and the reproductive_way_genome must not be scrambled.

    If the reproductive ways are equal, the reproductive_way_genome
    must be preserved and scrambled.
    */
    ReproductiveGenome scramble(
        const ReproductiveGenome&g1,
        const ReproductiveGenome&g2
    );

    BrainGenome
    scramble(const BrainGenome& g1,
             const BrainGenome& g2);

    EmbryoBodyGenome
    scramble(const EmbryoBodyGenome& g1,
             const EmbryoBodyGenome& g2);

    EmbryoMorphologyGenome
    scramble(const EmbryoMorphologyGenome& g1,
             const EmbryoMorphologyGenome& g2);
             
    PropertiesContainer crossover(const PropertiesContainer&c1, const PropertiesContainer&c2);
    CoreGenome crossover(const CoreGenome&g1, const CoreGenome&g2);
    CreatureGenome crossover(const CreatureGenome& g1, const CreatureGenome& g2);
    EmbryoGenome crossover(const EmbryoGenome& g1, const EmbryoGenome& g2);
    Genome crossover(const Genome& g1, const Genome& g2);
}