#include <karkinolution/organism/entities/creature/constructor.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/utils/namegenerator.hpp>

namespace {
	inline constexpr float NEONATE_FRACTION = 0.08f;
} // namespace

Creature CreatureConstructor::from_blueprint(const CreatureBlueprint &blueprint) {
	const CreatureSpecies specie = blueprint.specie.value_or(CreatureSpeciesF::choice());

	Genome      genome = global_creature_genomes.get_genome(specie);
	const auto &cg     = genome.creature_genome;
	const auto &bg     = genome.brain_genome;

	const uint64_t    id       = blueprint.id.value_or(RandomGenerators::generate<uint64_t>());
	const std::string name     = blueprint.name.value_or(gen_name());
	const Gender      gender   = blueprint.gender.value_or(GenderF::choice());
	const Vec3        position = blueprint.position.value_or(Vec3{0.0, 0.0, 0.0});

	const float initial_max_energy   = genome.calculate_embryo_average_max_energy();
	const float initial_energy_value = initial_max_energy * NEONATE_FRACTION;
	const float initial_max_reserved = cg.metabolism.max_reserved_energy * NEONATE_FRACTION;

	Metabolism metabolism{
		Energy{initial_energy_value, initial_max_energy},
		Energy{initial_energy_value * 0.5f, initial_max_reserved},
		Diet{cg.metabolism.diet},
	};

	const float initial_max_life = genome.calculate_embryo_average_life();

	Vital vital{
		.life     = Life{initial_max_life * NEONATE_FRACTION, initial_max_life},
		.immunity = cg.vital.average_immunity,
		.health   = cg.vital.average_health,
	};

	Morphology morphology{
		.size =
			Size{
				.lateral = Lateral{cg.morphology.average_lateral.value * NEONATE_FRACTION},
				.height  = Height{cg.morphology.average_height.value * NEONATE_FRACTION},
				.back    = Back{cg.morphology.average_back.value * NEONATE_FRACTION},

			},
		.mass = Mass{cg.morphology.average_mass.value * NEONATE_FRACTION},
	};

	SkeletonStructure skeleton{
		.shared_volume = cg.skeleton.shared_volume,
		.bone          = Bone{cg.skeleton.average_bones.value * NEONATE_FRACTION},
		.quality       = cg.skeleton.average_bone_quality,
	};

	MuscleStructure muscle{
		.muscle        = Muscle{cg.muscle.average_muscles.value * NEONATE_FRACTION},
		.efficiency    = cg.muscle.average_muscle_efficiency,
		.quality       = cg.muscle.average_muscle_quality,
		.shared_volume = cg.muscle.shared_volume,
	};

	ReproductiveState rep_state{
		gender,
		cg.reproductive.reproductive_way(),
		metabolism.energy,
		vital.health,
	};

	BodyReproductive reproductive{
		.fertility = FertilityCooldown{0, cg.reproductive.fertility_limit},
		.state     = std::move(rep_state),
	};

	Body body{
		.metabolism   = std::move(metabolism),
		.vital        = std::move(vital),
		.reproductive = std::move(reproductive),
		.morphology   = std::move(morphology),
		.skeleton     = std::move(skeleton),
		.muscle       = std::move(muscle),
		.age          = Age{0.0f},
	};

	Brain brain{};
	brain.sociability = bg.average_sociability;

	Ontology ontology{
		.gender = gender,
		.name   = name,
		.id     = id,
	};

	return Creature{std::move(genome),
					std::move(brain),
					std::move(ontology),
					std::move(body),
					position};
}
