#include "karkinolution/organism/entities/properties/properties.hpp"

#include <cassert>
#include <karkinolution/core/error.hpp>
#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/genetics/disturbs.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>

namespace Karkinolution::Organisms::Entities::Genetic {

	bool GenomeDatabase::exists_specie(Creature::CreatureSpecies c_s) const {
		return g_creatures.contains(c_s) && g_core.contains(c_s) && g_brain.contains(c_s)
			&& g_embryos.contains(c_s);
	}

	Genome GenomeDatabase::get_genome(Creature::CreatureSpecies c_s) const {
		if (!exists_specie(c_s)) {
			throw SimulationError("Genomes of species was not implemented (all)");
		}
		return Genome{.creature_genome = g_creatures.at(c_s),
					  .embryo_genome   = g_embryos.at(c_s),
					  .core_genome     = g_core.at(c_s),
					  .brain_genome    = g_brain.at(c_s)};
	}

	void GenomeDatabase::add(Creature::CreatureSpecies c_s, CreatureGenome &&g) {
		g_creatures.emplace(c_s, std::move(g));
	}

	void GenomeDatabase::add(Creature::CreatureSpecies c_s, EmbryoGenome &&g) {
		g_embryos.emplace(c_s, g);
	}

	void GenomeDatabase::add(Creature::CreatureSpecies c_s, CoreGenome &&g) {
		g_core.emplace(c_s, std::move(g));
	}

	void GenomeDatabase::add(Creature::CreatureSpecies c_s, BrainGenome &&g) {
		g_brain.emplace(c_s, g);
	}

	void Populate::populate_crocodile(GenomeDatabase &creature_genomes) {
		using namespace CreatureGenomes;
		creature_genomes.add(
			Creature::CreatureSpecies::CROCODILE,
			CreatureGenome{
				.metabolism =
					Metabolism::MetabolismGenome{
						.max_energy          = 100.0f,
						.max_reserved_energy = 40.0f,
						.diet                = Creature::Diet{NormalizedValue<float>{0.05f},
                                               NormalizedValue<float>{0.85f}},
						.growth_rates        = Metabolism::GrowthRates{.max_energy          = 0.30f,
																	   .energy              = 0.45f,
																	   .max_reserved_energy = 0.25f}},
				.vital = Vital::VitalGenome{.max_life         = 100.0f,
											.average_immunity = Immunity{0.65f},
											.average_health   = Health{0.85f},
											.average_age      = Age{50.0f},
											.growth_rates     = Vital::GrowthRates{.life = 0.30f}},

				.morphology   = Morphology::MorphologyGenome{.average_lateral = Lateral{2.20f},
															 .average_back    = Depth{1.10f},
															 .average_height  = Height{0.80f},
															 .average_mass    = Mass{1.25f}},
				.reproductive = Reproduction::ReproductiveGenome{
					.reproductive_way_genome = Reproduction::OviparousOrganismGenome{},
					.average_children_count  = 40,
					.average_gestation_limit = 0,
					.fertility_limit         = 2,
					.reproduction_cost       = 0.25f}});

		creature_genomes.add(
			Creature::CreatureSpecies::CROCODILE,
			EmbryoGenome{.body       = EmbryoGenomes::BodyGenome{.average_max_energy = 0.55f,
																 .average_max_life   = 0.45f,
																 .average_health     = 0.60f},
						 .morphology = EmbryoGenomes::Morphology{.volume_growth  = 0.18f,
																 .average_volume = 0.30f}});

		Properties::PropertiesContainer properties;

		properties.add(GenericProperty{Properties::Capabilities::Move::SWIMM});
		properties.add(GenericProperty{Properties::Capabilities::Move::WALK});

		creature_genomes.add(Creature::CreatureSpecies::CROCODILE,
							 CoreGenome{.properties    = std::move(properties),
										.vision_radius = Creature::VisionRadius{20.0},
										.temperament   = Creature::Temperament::AGGRESSIVE,
										.specie        = Creature::CreatureSpecies::CROCODILE});

		creature_genomes.add(Creature::CreatureSpecies::CROCODILE,
							 BrainGenome{.average_sociability = 0.20f});
	}

	void Populate::populate_fish(GenomeDatabase &genome_database) {

		using namespace CreatureGenomes;
		genome_database.add(
			Creature::CreatureSpecies::FISH,
			CreatureGenome{
				.metabolism =
					Metabolism::MetabolismGenome{
						.max_energy          = 10.0f,
						.max_reserved_energy = 4.0f,
						.diet                = Creature::Diet{NormalizedValue<float>{0.20f},
                                               NormalizedValue<float>{0.70f}},
						.growth_rates        = Metabolism::GrowthRates{.max_energy          = 0.55f,
																	   .energy              = 0.70f,
																	   .max_reserved_energy = 0.45f}},
				.vital = Vital::VitalGenome{.max_life         = 9.0f,
											.average_immunity = Immunity{0.35f},
											.average_health   = Health{0.45f},
											.average_age      = Age{10.0f},
											.growth_rates     = Vital::GrowthRates{.life = 0.50f}},
				.morphology =
					Morphology::MorphologyGenome{
						.average_lateral = Lateral{1.20f},
						.average_back    = Depth{0.50f},
						.average_height  = Height{0.45f},
						.average_mass    = Mass{1.0f},
					},
				.reproductive = Reproduction::ReproductiveGenome{
					.reproductive_way_genome = Reproduction::OviparousOrganismGenome{},
					.average_children_count  = 1000,
					.average_gestation_limit = 0,
					.fertility_limit         = 10,
					.reproduction_cost       = 0.05f}});

		genome_database.add(
			Creature::CreatureSpecies::FISH,
			EmbryoGenome{.body = Genetic::EmbryoGenomes::BodyGenome{.average_max_energy = 0.35f,
																	.average_max_life   = 0.30f,
																	.average_health     = 0.40f},
						 .morphology =
							 Genetic::EmbryoGenomes::Morphology{.volume_growth  = 0.25f,
																.average_volume = 0.20f}});
		Properties::PropertiesContainer properties;

		properties.add(GenericProperty{Properties::Capabilities::Move::SWIMM});
		genome_database.add(Creature::CreatureSpecies::FISH,
							CoreGenome{.properties    = std::move(properties),
									   .vision_radius = Creature::VisionRadius{5.0},
									   .temperament   = Creature::Temperament::PASSIVE,
									   .specie        = Creature::CreatureSpecies::FISH});

		genome_database.add(Creature::CreatureSpecies::FISH,
							BrainGenome{.average_sociability = 0.70f});
	}

	void Populate::populate_crab(GenomeDatabase &genome_database) {
		using namespace CreatureGenomes;

		genome_database.add(
			Creature::CreatureSpecies::CRAB,
			CreatureGenome{
				.metabolism =
					Metabolism::MetabolismGenome{
						.max_energy          = 40.0f,
						.max_reserved_energy = 15.0f,
						.diet                = Creature::Diet{NormalizedValue<float>{0.45f},
                                               NormalizedValue<float>{0.35f}},
						.growth_rates        = Metabolism::GrowthRates{.max_energy          = 0.40f,
																	   .energy              = 0.60f,
																	   .max_reserved_energy = 0.35f}},
				.vital        = Vital::VitalGenome{.max_life         = 30.0f,
												   .average_immunity = Immunity{0.45f},
												   .average_health   = Health{0.55f},
												   .average_age      = Age{30.0f},


												   .growth_rates = Vital::GrowthRates{.life = 0.40f}},
				.morphology   = Morphology::MorphologyGenome{.average_lateral = Lateral{1.40f},
															 .average_back    = Depth{1.00f},
															 .average_height  = Height{0.55f},
															 .average_mass    = Mass{1.10f}},
				.reproductive = Reproduction::ReproductiveGenome{
					.reproductive_way_genome = Reproduction::OviparousOrganismGenome{},
					.average_children_count  = 1000000,
					.average_gestation_limit = 0,
					.fertility_limit         = 20,
					.reproduction_cost       = 0.03f}});

		genome_database.add(
			Creature::CreatureSpecies::CRAB,
			EmbryoGenome{.body = Genetic::EmbryoGenomes::BodyGenome{.average_max_energy = 0.45f,
																	.average_max_life   = 0.35f,
																	.average_health     = 0.50f},
						 .morphology =
							 Genetic::EmbryoGenomes::Morphology{.volume_growth  = 0.22f,
																.average_volume = 0.25f}});

		Properties::PropertiesContainer properties;
		properties.add(GenericProperty{Properties::Capabilities::Move::WALK});
		genome_database.add(Creature::CreatureSpecies::CRAB,
							CoreGenome{.properties    = std::move(properties),
									   .vision_radius = Creature::VisionRadius{2.0},
									   .temperament   = Creature::Temperament::NEUTRAL,
									   .specie        = Creature::CreatureSpecies::CRAB});

		genome_database.add(Creature::CreatureSpecies::CRAB,
							BrainGenome{.average_sociability = 0.40f});
	}

	void Populate::populate_hippopotamus(GenomeDatabase &genome_database) {
		using namespace CreatureGenomes;

		genome_database.add(
			Creature::CreatureSpecies::HIPPOPOTAMUS,
			CreatureGenome{
				.metabolism =
					Metabolism::MetabolismGenome{
						.max_energy          = 85.0f,
						.max_reserved_energy = 35.0f,
						.diet                = Creature::Diet{NormalizedValue<float>{0.80f},
                                               NormalizedValue<float>{0.05f}},
						.growth_rates        = Metabolism::GrowthRates{.max_energy          = 0.25f,
																	   .energy              = 0.35f,
																	   .max_reserved_energy = 0.20f}},
				.vital = Vital::VitalGenome{.max_life         = 150.0f,
											.average_immunity = Immunity{0.70f},
											.average_health   = Health{0.90f},
											.average_age      = Age{60.0f},
											.growth_rates     = Vital::GrowthRates{.life = 0.25f}},
				.morphology =
					Morphology::MorphologyGenome{
						.average_lateral = Lateral{1.80f},
						.average_back    = Depth{1.40f},
						.average_height  = Height{1.30f},
						.average_mass    = Mass{1.60f},
					},
				.reproductive = Reproduction::ReproductiveGenome{
					.reproductive_way_genome = Reproduction::ViviparousOrganismGenome{},
					.average_children_count  = 1,
					.average_gestation_limit = 8,
					.fertility_limit         = 1,
					.reproduction_cost       = 0.35f}});

		genome_database.add(
			Creature::CreatureSpecies::HIPPOPOTAMUS,
			EmbryoGenome{.body = Genetic::EmbryoGenomes::BodyGenome{.average_max_energy = 0.65f,
																	.average_max_life   = 0.50f,
																	.average_health     = 0.70f},
						 .morphology =
							 Genetic::EmbryoGenomes::Morphology{.volume_growth  = 0.15f,
																.average_volume = 0.35f}});

		Properties::PropertiesContainer properties;

		properties.add(GenericProperty{Properties::Capabilities::Move::SWIMM});
		properties.add(GenericProperty{Properties::Capabilities::Move::WALK});

		genome_database.add(Creature::CreatureSpecies::HIPPOPOTAMUS,
							CoreGenome{.properties    = std::move(properties),
									   .vision_radius = Creature::VisionRadius{10.0},
									   .temperament   = Creature::Temperament::TERRITORIAL,
									   .specie        = Creature::CreatureSpecies::HIPPOPOTAMUS});
		genome_database.add(Creature::CreatureSpecies::HIPPOPOTAMUS,
							BrainGenome{.average_sociability = 0.25f});
	}

} // namespace Karkinolution::Organisms::Entities::Genetic
