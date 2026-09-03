#include <cassert>
#include <karkinolution/core/error.hpp>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/genetics/disturbs.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/utils/k_random.hpp>

namespace {

using MetabolismTrait           = Genomes::CreatureGenomes::Metabolism::Trait;
using MetabolismRelation        = Genomes::CreatureGenomes::Metabolism::Relation;
using MetabolismTransformations = Genomes::CreatureGenomes::Metabolism::Transformations;

using VitalTrait           = Genomes::CreatureGenomes::Vital::Trait;
using VitalTransformations = Genomes::CreatureGenomes::Vital::Transformations;

using MuscleTrait           = Genomes::CreatureGenomes::MuscleStructure::Trait;
using MuscleTransformations = Genomes::CreatureGenomes::MuscleStructure::Transformations;

MetabolismTransformations
metabolism_transformations(float max_reserved_energy, float max_energy, float energy) {
	return MetabolismTransformations{std::flat_map<Genomes::Resource,
												   std::vector<MetabolismRelation>>{
		{Genomes::Resource::RESERVED_ENERGY,
		 {{Genomes::Resource::RESERVED_ENERGY,
		   MetabolismTrait::MAX_RESERVED_ENERGY,
		   Efficiency{max_reserved_energy}},
		  {Genomes::Resource::RESERVED_ENERGY, MetabolismTrait::MAX_ENERGY, Efficiency{max_energy}},
		  {Genomes::Resource::RESERVED_ENERGY, MetabolismTrait::ENERGY, Efficiency{energy}}}}}};
}

VitalTransformations vital_transformations(float max_life) {
	VitalTransformations transformations;

	transformations.add(
		Genomes::Resource::RESERVED_ENERGY,
		{Genomes::Resource::RESERVED_ENERGY, VitalTrait::MAX_LIFE, Efficiency{max_life}});

	return transformations;
}

MuscleTransformations muscle_transformations(float muscle) {
	MuscleTransformations transformations;

	transformations.add(
		Genomes::Resource::RESERVED_ENERGY,
		{Genomes::Resource::RESERVED_ENERGY, MuscleTrait::MUSCLE, Efficiency{muscle}});

	return transformations;
}

using SkeletonTransformations = Genomes::CreatureGenomes::SkeletonStructure::Transformations;

SkeletonTransformations skeleton_transformations(float bone) {
	SkeletonTransformations transformations;

	transformations.add(Genomes::Resource::RESERVED_ENERGY,
						{Genomes::Resource::RESERVED_ENERGY,
						 Genomes::CreatureGenomes::SkeletonStructure::Trait::BONES,
						 Efficiency{bone}});

	return transformations;
}

PropertiesContainer movement_properties(bool can_walk, bool can_swim) {
	PropertiesContainer properties;


	if (can_swim) {
		properties.add(Properties::Capabilities::Move::SWIMM);
	} else if (can_walk) {
		properties.add(Properties::Capabilities::Move::WALK);
	}
	return properties;
}

} // namespace

CreatureSpecies CreatureSpeciesF::choice() {
	return Choices::choice({CreatureSpecies::CRAB,
							CreatureSpecies::CROCODILE,
							CreatureSpecies::FISH,
							CreatureSpecies::HIPPOPOTAMUS});
}

bool CreatureGenomes::exists_specie(CreatureSpecies c_s) const {
	return g_creatures.contains(c_s) && g_core.contains(c_s) && g_brain.contains(c_s)
		&& g_embryos.contains(c_s);
}

Genome CreatureGenomes::get_genome(CreatureSpecies c_s) const {
	if (!exists_specie(c_s)) {
		throw SimulationError("Genomes of species was not implemented (all)");
	}
	return Genome{.creature_genome = g_creatures.at(c_s),
				  .embryo_genome   = g_embryos.at(c_s),
				  .core_genome     = g_core.at(c_s),
				  .brain_genome    = g_brain.at(c_s)};
}

void CreatureGenomes::add(CreatureSpecies c_s, CreatureGenome &&g) {
	g_creatures.emplace(c_s, std::move(g));
}

void CreatureGenomes::add(CreatureSpecies c_s, EmbryoGenome &&g) {
	g_embryos.emplace(c_s, std::move(g));
}

void CreatureGenomes::add(CreatureSpecies c_s, CoreGenome &&g) {
	g_core.emplace(c_s, std::move(g));
}

void CreatureGenomes::add(CreatureSpecies c_s, BrainGenome &&g) {
	g_brain.emplace(c_s, std::move(g));
}

void Populate::populate_crocodile(CreatureGenomes &creature_genomes) {
	using namespace Genomes::CreatureGenomes;

	creature_genomes.add(
		CreatureSpecies::CROCODILE,
		CreatureGenome{
			.metabolism =
				Metabolism::MetabolismGenome{
					.max_energy          = 100.0f,
					.max_reserved_energy = 40.0f,
					.diet = Diet{NormalizedValue<float>{0.05f}, NormalizedValue<float>{0.85f}},
					.transformations = metabolism_transformations(0.75f, 0.70f, 0.80f),
					.growth_rates    = Metabolism::GrowthRates{.max_energy          = 0.30f,
															   .energy              = 0.45f,
															   .max_reserved_energy = 0.25f}},
			.vital = Vital::VitalGenome{.max_life         = 100.0f,
										.average_immunity = Immunity{0.65f},
										.average_health   = Health{0.85f},
										.average_age      = Age{50.0f},
										.transformations  = vital_transformations(0.65f),
										.growth_rates     = Vital::GrowthRates{.life = 0.30f}},
			.muscle =
				MuscleStructure::MuscleGenome{.shared_volume             = SharedVolume{0.55f},
											  .average_muscles           = Muscle{27.0f},
											  .average_muscle_efficiency = MuscleEfficiency{0.80f},
											  .average_muscle_quality    = MuscleQuality{0.85f},
											  .growth_rates =
												  MuscleStructure::GrowthRates{.muscle = 0.40f},
											  .transformations = muscle_transformations(0.80f)},
			.skeleton =
				SkeletonStructure::SkeletonGenome{.shared_volume        = SharedVolume{0.60f},
												  .average_bones        = Bone{30.0f},
												  .average_bone_quality = SkeletonQuality{0.85f},
												  .growth               = GrowthRate{0.30f},
												  .transformations =
													  skeleton_transformations(0.80f)},
			.morphology   = Morphology::MorphologyGenome{.average_lateral = Lateral{2.20f},
														 .average_back    = Back{1.10f},
														 .average_height  = Height{0.80f},
														 .average_mass    = Mass{1.25f},
														 .transformations = {}},
			.reproductive = Reproduction::ReproductiveGenome{
				.reproductive_way_genome = Reproduction::OviparousOrganismGenome{},
				.average_children_count  = 40,
				.average_gestation_limit = 0,
				.fertility_limit         = 2,
				.reproduction_cost       = 0.25f}});

	creature_genomes.add(
		CreatureSpecies::CROCODILE,
		EmbryoGenome{.body       = Genomes::EmbryoGenomes::BodyGenome{.average_max_energy = 0.55f,
																	  .average_max_life   = 0.45f,
																	  .average_health     = 0.60f},
					 .morphology = Genomes::EmbryoGenomes::Morphology{.volume_growth  = 0.18f,
																	  .average_volume = 0.30f}});

	creature_genomes.add(CreatureSpecies::CROCODILE,
						 CoreGenome{.properties    = movement_properties(true, false),
									.vision_radius = VisionRadius{20.0},
									.temperament   = Temperament::AGGRESSIVE,
									.specie        = CreatureSpecies::CROCODILE});

	creature_genomes.add(CreatureSpecies::CROCODILE, BrainGenome{.average_sociability = 0.20f});
}

void Populate::populate_fish(CreatureGenomes &creature_genomes) {
	using namespace Genomes::CreatureGenomes;

	creature_genomes.add(
		CreatureSpecies::FISH,
		CreatureGenome{
			.metabolism =
				Metabolism::MetabolismGenome{
					.max_energy          = 10.0f,
					.max_reserved_energy = 4.0f,
					.diet = Diet{NormalizedValue<float>{0.20f}, NormalizedValue<float>{0.70f}},
					.transformations = metabolism_transformations(0.60f, 0.55f, 0.75f),
					.growth_rates    = Metabolism::GrowthRates{.max_energy          = 0.55f,
															   .energy              = 0.70f,
															   .max_reserved_energy = 0.45f}},
			.vital = Vital::VitalGenome{.max_life         = 9.0f,
										.average_immunity = Immunity{0.35f},
										.average_health   = Health{0.45f},
										.average_age      = Age{10.0f},
										.transformations  = vital_transformations(0.50f),
										.growth_rates     = Vital::GrowthRates{.life = 0.50f}},
			.muscle =
				MuscleStructure::MuscleGenome{.shared_volume             = SharedVolume{0.70f},
											  .average_muscles           = Muscle{1.0f},
											  .average_muscle_efficiency = MuscleEfficiency{0.55f},
											  .average_muscle_quality    = MuscleQuality{0.45f},
											  .growth_rates =
												  MuscleStructure::GrowthRates{.muscle = 0.65f},
											  .transformations = muscle_transformations(0.65f)},
			.skeleton =
				SkeletonStructure::SkeletonGenome{.shared_volume        = SharedVolume{0.15f},
												  .average_bones        = Bone{0.80f},
												  .average_bone_quality = SkeletonQuality{0.35f},
												  .growth               = GrowthRate{0.50f},
												  .transformations =
													  skeleton_transformations(0.65f)},
			.morphology   = Morphology::MorphologyGenome{.average_lateral = Lateral{1.20f},
														 .average_back    = Back{0.50f},
														 .average_height  = Height{0.45f},
														 .average_mass    = Mass{1.0f},
														 .transformations = {}},
			.reproductive = Reproduction::ReproductiveGenome{
				.reproductive_way_genome = Reproduction::OviparousOrganismGenome{},
				.average_children_count  = 1000,
				.average_gestation_limit = 0,
				.fertility_limit         = 10,
				.reproduction_cost       = 0.05f}});

	creature_genomes.add(
		CreatureSpecies::FISH,
		EmbryoGenome{.body       = Genomes::EmbryoGenomes::BodyGenome{.average_max_energy = 0.35f,
																	  .average_max_life   = 0.30f,
																	  .average_health     = 0.40f},
					 .morphology = Genomes::EmbryoGenomes::Morphology{.volume_growth  = 0.25f,
																	  .average_volume = 0.20f}});

	creature_genomes.add(CreatureSpecies::FISH,
						 CoreGenome{.properties    = movement_properties(true, false),
									.vision_radius = VisionRadius{5.0},
									.temperament   = Temperament::PASSIVE,
									.specie        = CreatureSpecies::FISH});

	creature_genomes.add(CreatureSpecies::FISH, BrainGenome{.average_sociability = 0.70f});
}

void Populate::populate_crab(CreatureGenomes &creature_genomes) {
	using namespace Genomes::CreatureGenomes;

	creature_genomes.add(
		CreatureSpecies::CRAB,
		CreatureGenome{
			.metabolism =
				Metabolism::MetabolismGenome{
					.max_energy          = 40.0f,
					.max_reserved_energy = 15.0f,
					.diet = Diet{NormalizedValue<float>{0.45f}, NormalizedValue<float>{0.35f}},
					.transformations = metabolism_transformations(0.65f, 0.60f, 0.75f),
					.growth_rates    = Metabolism::GrowthRates{.max_energy          = 0.40f,
															   .energy              = 0.60f,
															   .max_reserved_energy = 0.35f}},
			.vital = Vital::VitalGenome{.max_life         = 30.0f,
										.average_immunity = Immunity{0.45f},
										.average_health   = Health{0.55f},
										.average_age      = Age{30.0f},
										.transformations  = vital_transformations(0.55f),
										.growth_rates     = Vital::GrowthRates{.life = 0.40f}},
			.muscle =
				MuscleStructure::MuscleGenome{.shared_volume             = SharedVolume{0.65f},
											  .average_muscles           = Muscle{7.5f},
											  .average_muscle_efficiency = MuscleEfficiency{0.65f},
											  .average_muscle_quality    = MuscleQuality{0.55f},
											  .growth_rates =
												  MuscleStructure::GrowthRates{.muscle = 0.50f},
											  .transformations = muscle_transformations(0.70f)},
			.skeleton =
				SkeletonStructure::SkeletonGenome{.shared_volume        = SharedVolume{0.30f},
												  .average_bones        = Bone{5.0f},
												  .average_bone_quality = SkeletonQuality{0.65f},
												  .growth               = GrowthRate{0.40f},
												  .transformations =
													  skeleton_transformations(0.70f)},
			.morphology   = Morphology::MorphologyGenome{.average_lateral = Lateral{1.40f},
														 .average_back    = Back{1.00f},
														 .average_height  = Height{0.55f},
														 .average_mass    = Mass{1.10f},
														 .transformations = {}},
			.reproductive = Reproduction::ReproductiveGenome{
				.reproductive_way_genome = Reproduction::OviparousOrganismGenome{},
				.average_children_count  = 1000000,
				.average_gestation_limit = 0,
				.fertility_limit         = 20,
				.reproduction_cost       = 0.03f}});

	creature_genomes.add(
		CreatureSpecies::CRAB,
		EmbryoGenome{.body       = Genomes::EmbryoGenomes::BodyGenome{.average_max_energy = 0.45f,
																	  .average_max_life   = 0.35f,
																	  .average_health     = 0.50f},
					 .morphology = Genomes::EmbryoGenomes::Morphology{.volume_growth  = 0.22f,
																	  .average_volume = 0.25f}});

	creature_genomes.add(CreatureSpecies::CRAB,
						 CoreGenome{.properties    = movement_properties(false, true),
									.vision_radius = VisionRadius{2.0},
									.temperament   = Temperament::NEUTRAL,
									.specie        = CreatureSpecies::CRAB});

	creature_genomes.add(CreatureSpecies::CRAB, BrainGenome{.average_sociability = 0.40f});
}

void Populate::populate_hippopotamus(CreatureGenomes &creature_genomes) {
	using namespace Genomes::CreatureGenomes;

	creature_genomes.add(
		CreatureSpecies::HIPPOPOTAMUS,
		CreatureGenome{
			.metabolism =
				Metabolism::MetabolismGenome{
					.max_energy          = 85.0f,
					.max_reserved_energy = 35.0f,
					.diet = Diet{NormalizedValue<float>{0.80f}, NormalizedValue<float>{0.05f}},
					.transformations = metabolism_transformations(0.80f, 0.65f, 0.70f),
					.growth_rates    = Metabolism::GrowthRates{.max_energy          = 0.25f,
															   .energy              = 0.35f,
															   .max_reserved_energy = 0.20f}},
			.vital = Vital::VitalGenome{.max_life         = 150.0f,
										.average_immunity = Immunity{0.70f},
										.average_health   = Health{0.90f},
										.average_age      = Age{60.0f},
										.transformations  = vital_transformations(0.75f),
										.growth_rates     = Vital::GrowthRates{.life = 0.25f}},
			.muscle =
				MuscleStructure::MuscleGenome{.shared_volume             = SharedVolume{0.80f},
											  .average_muscles           = Muscle{20.0f},
											  .average_muscle_efficiency = MuscleEfficiency{0.75f},
											  .average_muscle_quality    = MuscleQuality{0.80f},
											  .growth_rates =
												  MuscleStructure::GrowthRates{.muscle = 0.35f},
											  .transformations = muscle_transformations(0.75f)},
			.skeleton =
				SkeletonStructure::SkeletonGenome{.shared_volume        = SharedVolume{0.90f},
												  .average_bones        = Bone{35.0f},
												  .average_bone_quality = SkeletonQuality{0.90f},
												  .growth               = GrowthRate{0.25f},
												  .transformations =
													  skeleton_transformations(0.75f)},
			.morphology   = Morphology::MorphologyGenome{.average_lateral = Lateral{1.80f},
														 .average_back    = Back{1.40f},
														 .average_height  = Height{1.30f},
														 .average_mass    = Mass{1.60f},
														 .transformations = {}},
			.reproductive = Reproduction::ReproductiveGenome{
				.reproductive_way_genome = Reproduction::ViviparousOrganismGenome{},
				.average_children_count  = 1,
				.average_gestation_limit = 8,
				.fertility_limit         = 1,
				.reproduction_cost       = 0.35f}});

	creature_genomes.add(
		CreatureSpecies::HIPPOPOTAMUS,
		EmbryoGenome{.body       = Genomes::EmbryoGenomes::BodyGenome{.average_max_energy = 0.65f,
																	  .average_max_life   = 0.50f,
																	  .average_health     = 0.70f},
					 .morphology = Genomes::EmbryoGenomes::Morphology{.volume_growth  = 0.15f,
																	  .average_volume = 0.35f}});
	creature_genomes.add(CreatureSpecies::HIPPOPOTAMUS,
						 CoreGenome{.properties    = movement_properties(true, true),
									.vision_radius = VisionRadius{10.0},
									.temperament   = Temperament::TERRITORIAL,
									.specie        = CreatureSpecies::HIPPOPOTAMUS});
	creature_genomes.add(CreatureSpecies::HIPPOPOTAMUS, BrainGenome{.average_sociability = 0.25f});
}
