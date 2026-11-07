#include "karkinolution/organism/entities/creature/creature.hpp"

#include <shared/functional/k_random.hpp>
#include <karkinolution/organism/entities/creature/constructor.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/utils/namegenerator.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Constructor {
	namespace {
		inline constexpr float NEONATE_FRACTION = 0.08f;
	} // namespace

	Karkinolution::Organisms::Entities::Creature::Creature
	CreatureConstructor::from_blueprint(const CreatureBlueprint &blueprint) {
		const auto specie = blueprint.specie.value_or(chooes_random_specie());

		Genetic::Genome genome = Genetic::global_creature_genomes.get_genome(specie);
		const auto     &cg     = genome.creature_genome;
		const auto     &bg     = genome.brain_genome;

		const auto        id       = blueprint.id.value_or(KShared::Core::Id::gen_id());
		const std::string name     = blueprint.name.value_or(gen_name());
		const auto        gender   = blueprint.gender.value_or(choose_random_gender());
		const Vec2        position = blueprint.position.value_or(Vec2{0.0, 0.0});

		const float initial_max_energy   = genome.calculate_embryo_average_max_energy();
		const float initial_energy_value = initial_max_energy * NEONATE_FRACTION;
		const float initial_max_reserved = cg.metabolism.max_reserved_energy * NEONATE_FRACTION;

		MetabolismPart metabolism{
			Energy{initial_energy_value, initial_max_energy},
			Energy{initial_energy_value * 0.5f, initial_max_reserved},
			Diet{cg.metabolism.diet},
		};

		const float initial_max_life = genome.calculate_embryo_average_life();

		VitalPart vital{
			.life     = Life{initial_max_life * NEONATE_FRACTION, initial_max_life},
			.immunity = cg.vital.average_immunity,
			.health   = cg.vital.average_health,
		};

		MorphologyPart morphology{
			.size =
				Size{
					.width  = Width{cg.morphology.average_width.value * NEONATE_FRACTION},
					.height = Height{cg.morphology.average_height.value * NEONATE_FRACTION},
				},
			.mass = Mass{cg.morphology.average_mass.value * NEONATE_FRACTION},
		};

		Reproduction::ReproductiveState rep_state{
			gender,
			cg.reproductive.reproductive_way(),
			metabolism.energy,
			vital.health,
		};

		ReproductivePart reproductive{
			.fertility = FertilityCooldown{0, cg.reproductive.fertility_limit},
			.state     = std::move(rep_state),
		};

		Body body{
			.metabolism   = metabolism,
			.vital        = vital,
			.morphology   = morphology,
			.reproduction = std::move(reproductive),
			.age          = Age{0.0f},
		};

		Brain::Brain brain{};
		brain.sociability = bg.average_sociability;

		Ontology ontology{
			.gender = gender,
			.name   = name,
			.id     = id,
		};

		return Karkinolution::Organisms::Entities::Creature::Creature{std::move(genome),
																	  std::move(brain),
																	  std::move(ontology),
																	  std::move(body),
																	  position};
	}
} // namespace Karkinolution::Organisms::Entities::Creature::Constructor
