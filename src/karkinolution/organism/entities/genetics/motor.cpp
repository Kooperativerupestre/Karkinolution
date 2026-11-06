#include "karkinolution/organism/reproduction/state/ontology.hpp"

#include <algorithm>
#include <cmath>
#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/disturbs.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/entities/genetics/motor.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution::Organisms::Entities::Genetic {
	namespace Motor {


		using CreatureMorphologyGenome = CreatureGenomes::Morphology::MorphologyGenome;
		using CreatureMetabolismGenome = CreatureGenomes::Metabolism::MetabolismGenome;
		using CreatureVitalGenome      = CreatureGenomes::Vital::VitalGenome;

		using EmbryoBodyGenome       = EmbryoGenomes::BodyGenome;
		using EmbryoMorphologyGenome = EmbryoGenomes::Morphology;

		using ReproductionOviparous  = CreatureGenomes::Reproduction::OviparousOrganismGenome;
		using ReproductionViviparous = CreatureGenomes::Reproduction::ViviparousOrganismGenome;

		using MetabolismGrowthRates = CreatureGenomes::Metabolism::GrowthRates;
		using VitalGrowthRates      = CreatureGenomes::Vital::GrowthRates;

		CreatureMorphologyGenome scramble(const CreatureMorphologyGenome &g1, const CreatureMorphologyGenome &g2) {


			return CreatureGenomes::Morphology::MorphologyGenome{
				.average_width  = Disturbs::scramble(g1.average_width.value,
                                                    g2.average_width.value,
                                                    Disturbs::StandardContexts::morphology_scramble),
				.average_height = Disturbs::scramble(
					g1.average_height.value,
					g2.average_height.value,
					Disturbs::ScrambleContext{.max_base            = 0.70f,
											  .min_base            = 1.30f,
											  .distance_multiplier = 1.5f,
											  .mutations_counts    = 2,
											  .suppress_context =
												  Disturbs::SuppressContext{.chance_of_v1_being_suppressed = 0.4f,
																			.chance_of_v2_being_suppressed = 0.4f,
																			.suppressed_multiplier         = 0.35f}}),
				.average_mass = Disturbs::scramble(
					g1.average_mass.value,
					g2.average_mass.value,
					Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
											  .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
											  .distance_multiplier = Disturbs::HIGH_DISTANCE_MULTIPLIER,
											  .suppress_context =
												  Disturbs::SuppressContext{.chance_of_v1_being_suppressed = 0.25f,
																			.chance_of_v2_being_suppressed = 0.25f,
																			.suppressed_multiplier         = 0.35f}})};
		}

		Karkinolution::Organisms::Entities::Creature::Diet
		scramble(const Karkinolution::Organisms::Entities::Creature::Diet &d1,
				 const Karkinolution::Organisms::Entities::Creature::Diet &d2) {
			auto new_diet =
				Creature::Diet{// scramble: mixes two values with randomness. Parameters:
							   // max_base/min_base: bounds for random multiplication factor
							   // distance_multiplier: scales the difference between inputs
							   Disturbs::scramble(d1[FoodHint::GRASS_MATTER],
												  d2[FoodHint::GRASS_MATTER],
												  Disturbs::ScrambleContext{.max_base = Disturbs::LOW_SCRAMBLE_MAX_BASE,
																			.min_base = Disturbs::LOW_SCRAMBLE_MIN_BASE,
																			.distance_multiplier = 0.75f}),
							   Disturbs::scramble(d1[FoodHint::RAW_MEAT],
												  d2[FoodHint::RAW_MEAT],
												  Disturbs::ScrambleContext{.max_base = Disturbs::LOW_SCRAMBLE_MAX_BASE,
																			.min_base = Disturbs::LOW_SCRAMBLE_MIN_BASE,
																			.distance_multiplier = 0.75f})};

			return new_diet;
		}

		CreatureGenomes::Metabolism::MetabolismGenome
		scramble(const CreatureGenomes::Metabolism::MetabolismGenome &g1,
				 const CreatureGenomes::Metabolism::MetabolismGenome &g2) {


			auto new_growth_rates = MetabolismGrowthRates{
				.max_energy = Disturbs::scramble(g1.growth_rates.max_energy.value,
												 g2.growth_rates.max_energy.value,
												 Disturbs::ScrambleContext{.max_base            = 1.15f,
																		   .min_base            = 0.85f,
																		   .distance_multiplier = 0.7f,
																		   .inheritance_bias    = 1.0f / 1.9f,
																		   .mutations_counts    = 2}),
				.energy     = Disturbs::scramble(
                    g1.growth_rates.energy.value,
                    g2.growth_rates.energy.value,
                    Disturbs::ScrambleContext{.max_base            = 1.15f,
												  .min_base            = 0.85f,
												  .distance_multiplier = Disturbs::MEDIUM_DISTANCE_MULTIPLIER,
												  .inheritance_bias    = 1.0f / 2.1f}),
				.max_reserved_energy = Disturbs::scramble(g1.growth_rates.max_reserved_energy.value,
														  g2.growth_rates.max_reserved_energy.value,
														  Disturbs::StandardContexts::metabolism_growth_rates)};

			return CreatureGenomes::Metabolism::MetabolismGenome{
				.max_energy = Disturbs::scramble(
					g1.max_energy,
					g2.max_energy,
					Disturbs::ScrambleContext{.max_base = 0.80f, .min_base = 1.20f, .distance_multiplier = 0.2f}),
				.max_reserved_energy = Disturbs::scramble(
					g1.max_reserved_energy,
					g2.max_reserved_energy,
					Disturbs::ScrambleContext{.max_base = 0.91f, .min_base = 1.09f, .distance_multiplier = 0.1f}),
				.diet         = scramble(g1.diet, g2.diet),
				.growth_rates = new_growth_rates};
		}

		ReproductionViviparous scramble(const ReproductionViviparous &g1, const ReproductionViviparous &g2) {
			return {};
		}

		CreatureGenomes::Reproduction::OviparousOrganismGenome
		scramble(const CreatureGenomes::Reproduction::OviparousOrganismGenome &g1,
				 const CreatureGenomes::Reproduction::OviparousOrganismGenome &g2) {
			return {};
		}

		CreatureGenomes::Reproduction::ReproductiveGenome scramble(const ReproductiveGenome &g1,
																   const ReproductiveGenome &g2) {
			const auto children =
				std::max(1,
						 static_cast<int>(std::lround(Disturbs::scramble(
							 g1.average_children_count,
							 g2.average_children_count,
							 Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
													   .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
													   .distance_multiplier = Disturbs::LOW_DISTANCE_MULTIPLIER}))));

			const int fertility_limit =
				std::max(1,
						 static_cast<int>(std::lround(Disturbs::scramble(
							 g1.fertility_limit,
							 g2.fertility_limit,
							 Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
													   .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
													   .distance_multiplier = Disturbs::LOW_DISTANCE_MULTIPLIER}))));

			const float reproduction_cost =
				Disturbs::scramble(g1.reproduction_cost.value(),
								   g2.reproduction_cost.value(),
								   Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
															 .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
															 .distance_multiplier = Disturbs::LOW_DISTANCE_MULTIPLIER});
			const int average_gestation_limit =
				std::max(1,
						 static_cast<int>(std::lround(Disturbs::scramble(
							 g1.average_gestation_limit,
							 g2.average_gestation_limit,
							 Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
													   .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
													   .distance_multiplier = Disturbs::LOW_DISTANCE_MULTIPLIER}))));


			if (g1.reproductive_way() != g2.reproductive_way()) {
				const auto chosen = KRandom::choice({g1, g2});

				return ReproductiveGenome{
					.reproductive_way_genome = chosen.reproductive_way_genome,
					.average_children_count  = children,
					.average_gestation_limit = average_gestation_limit,
					.fertility_limit         = fertility_limit,
					.reproduction_cost       = reproduction_cost,

				};
			}

			std::variant<ReproductionViviparous, ReproductionOviparous> new_way_genome;
			if (g1.reproductive_way() == Reproduction::State::Ontology::ReproductiveWays::OVIPAROUS) {
				new_way_genome = scramble(std::get<ReproductionOviparous>(g1.reproductive_way_genome),
										  std::get<ReproductionOviparous>(g1.reproductive_way_genome));
			} else { // reproductive_way == VIVIPAROUS
				new_way_genome = scramble(std::get<ReproductionViviparous>(g1.reproductive_way_genome),
										  std::get<ReproductionViviparous>(g2.reproductive_way_genome));
			}

			return {.reproductive_way_genome = new_way_genome,
					.average_children_count  = children,
					.average_gestation_limit = average_gestation_limit,
					.fertility_limit         = fertility_limit,
					.reproduction_cost       = reproduction_cost};
		}

		CreatureGenomes::Vital::VitalGenome scramble(const CreatureGenomes::Vital::VitalGenome &g1,
													 const CreatureGenomes::Vital::VitalGenome &g2) {


			auto new_growth_rates = VitalGrowthRates{
				.life = Disturbs::scramble(
					g1.growth_rates.life.value,
					g2.growth_rates.life.value,
					Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
											  .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
											  .distance_multiplier = Disturbs::MEDIUM_DISTANCE_MULTIPLIER})};

			return CreatureVitalGenome{
				.max_life = Disturbs::scramble(
					g1.max_life,
					g2.max_life,
					Disturbs::ScrambleContext{.max_base            = 1.10f,
											  .min_base            = 0.90f,
											  .distance_multiplier = Disturbs::MEDIUM_DISTANCE_MULTIPLIER}),
				.average_immunity = Disturbs::scramble(
					g1.average_immunity.value(),
					g2.average_immunity.value(),
					Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
											  .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
											  .distance_multiplier = Disturbs::LOW_DISTANCE_MULTIPLIER}),
				.average_health = Disturbs::scramble(
					g1.average_health.value(),
					g2.average_health.value(),
					Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
											  .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
											  .distance_multiplier = Disturbs::LOW_DISTANCE_MULTIPLIER}),
				.average_age = Disturbs::scramble(
					g1.average_age.value,
					g2.average_age.value,
					Disturbs::ScrambleContext{.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
											  .min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
											  .distance_multiplier = Disturbs::HIGH_DISTANCE_MULTIPLIER}),
				.growth_rates = new_growth_rates};
		}

		BrainGenome scramble(const BrainGenome &g1, const BrainGenome &g2) {
			return BrainGenome{.average_sociability =
								   Disturbs::scramble(g1.average_sociability.value(),
													  g2.average_sociability.value(),
													  Disturbs::ScrambleContext{.max_base            = 1.30f,
																				.min_base            = 0.70f,
																				.distance_multiplier = 1.0f,
																				.mutations_counts    = 3})};
		}

		EmbryoGenomes::BodyGenome scramble(const EmbryoGenomes::BodyGenome &g1, const EmbryoGenomes::BodyGenome &g2) {
			// Embryonic reserves are strongly constrained by development.  Keep the
			// mutation window small while allowing the parental difference to matter.
			const Disturbs::ScrambleContext body_context{
				.max_base            = Disturbs::LOW_SCRAMBLE_MAX_BASE,
				.min_base            = Disturbs::LOW_SCRAMBLE_MIN_BASE,
				.distance_multiplier = Disturbs::LOW_DISTANCE_MULTIPLIER,
				.mutations_counts    = 1,
				.suppress_context    = Disturbs::SuppressContext{.chance_of_v1_being_suppressed = 0.20f,
																 .chance_of_v2_being_suppressed = 0.20f,
																 .suppressed_multiplier         = 0.40f}};

			return EmbryoBodyGenome{
				.average_max_energy =
					Disturbs::scramble(g1.average_max_energy.value(), g2.average_max_energy.value(), body_context),
				.average_max_life =
					Disturbs::scramble(g1.average_max_life.value(), g2.average_max_life.value(), body_context),
				.average_health =
					Disturbs::scramble(g1.average_health.value(), g2.average_health.value(), body_context)};
		}

		EmbryoGenomes::Morphology scramble(const EmbryoGenomes::Morphology &g1, const EmbryoGenomes::Morphology &g2) {
			// Growth rate is more plastic during embryogenesis than the final size;
			// final area therefore uses the same restrained morphology context used
			// by the adult genome.
			return EmbryoMorphologyGenome{
				.area_growth = Disturbs::scramble(
					g1.area_growth,
					g2.area_growth,
					Disturbs::ScrambleContext{.max_base            = 1.15f,
											  .min_base            = 0.85f,
											  .distance_multiplier = Disturbs::MEDIUM_DISTANCE_MULTIPLIER,
											  .mutations_counts    = 1,
											  .suppress_context =
												  Disturbs::SuppressContext{.chance_of_v1_being_suppressed = 0.18f,
																			.chance_of_v2_being_suppressed = 0.18f,
																			.suppressed_multiplier         = 0.50f}}),
				.average_area = Disturbs::scramble(g1.average_area.value(),
												   g2.average_area.value(),
												   Disturbs::StandardContexts::morphology_scramble)};
		}

		Genome crossover(const Genome &g1, const Genome &g2) {
			return Genome{
				.creature_genome = crossover(g1.creature_genome, g2.creature_genome),
				.embryo_genome   = crossover(g1.embryo_genome, g2.embryo_genome),
				.core_genome     = crossover(g1.core_genome, g2.core_genome),
				.brain_genome    = scramble(g1.brain_genome, g2.brain_genome),
			};
		}

	} // namespace Motor
} // namespace Karkinolution::Organisms::Entities::Genetic
