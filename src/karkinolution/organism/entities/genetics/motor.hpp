#pragma once
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Genetic {
				namespace Motor {
					using CreatureMorphologyGenome = CreatureGenomes::Morphology::MorphologyGenome;
					using CreatureMetabolismGenome = CreatureGenomes::Metabolism::MetabolismGenome;
					using CreatureVitalGenome      = CreatureGenomes::Vital::VitalGenome;


					using EmbryoBodyGenome       = EmbryoGenomes::BodyGenome;
					using EmbryoMorphologyGenome = EmbryoGenomes::Morphology;

					using ReproductionOviparous =
						CreatureGenomes::Reproduction::OviparousOrganismGenome;
					using ReproductionViviparous =
						CreatureGenomes::Reproduction::ViviparousOrganismGenome;
					using ReproductiveGenome = CreatureGenomes::Reproduction::ReproductiveGenome;


					Creature::Diet scramble(const Creature::Diet &d1, const Creature::Diet &d2);

					CreatureMorphologyGenome scramble(const CreatureMorphologyGenome &g1,
													  const CreatureMorphologyGenome &g2);

					CreatureVitalGenome scramble(const CreatureVitalGenome &g1,
												 const CreatureVitalGenome &g2);


					CreatureMetabolismGenome scramble(const CreatureMetabolismGenome &g1,
													  const CreatureMetabolismGenome &g2);

					ReproductionOviparous scramble(const ReproductionOviparous &g1,
												   const ReproductionOviparous &g2);


					ReproductionViviparous scramble(const ReproductionViviparous &g1,
													const ReproductionViviparous &g2);


					/*
					scramble(ReproductiveGenome)

					If the reproductive ways are different, one must be chosen,
					and the reproductive_way_genome must not be scrambled.

					If the reproductive ways are equal, the reproductive_way_genome
					must be preserved and scrambled.
					*/
					ReproductiveGenome scramble(const ReproductiveGenome &g1,
												const ReproductiveGenome &g2);

					BrainGenome scramble(const BrainGenome &g1, const BrainGenome &g2);

					EmbryoBodyGenome scramble(const EmbryoBodyGenome &g1,
											  const EmbryoBodyGenome &g2);

					EmbryoMorphologyGenome scramble(const EmbryoMorphologyGenome &g1,
													const EmbryoMorphologyGenome &g2);

					Properties::PropertiesContainer
								   crossover(const Properties::PropertiesContainer &c1,
											 const Properties::PropertiesContainer &c2);
					CoreGenome     crossover(const CoreGenome &g1, const CoreGenome &g2);
					CreatureGenome crossover(const CreatureGenome &g1, const CreatureGenome &g2);
					EmbryoGenome   crossover(const EmbryoGenome &g1, const EmbryoGenome &g2);
					Genome         crossover(const Genome &g1, const Genome &g2);


				} // namespace Motor
			} // namespace Genetic
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
