#pragma once

#include "karkinolution/math/physic/vec/model.hpp"

#include <cstdint>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/creature/brain/intents.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/organism/stats.hpp>

using OrganismStats::Energy;
using OrganismStats::Life;
using OrganismStats::Body::Strength;
using OrganismStats::Health::Health;
using OrganismStats::Health::Immunity;
using OrganismStats::Muscle::Muscle;
using OrganismStats::Muscle::MuscleEfficiency;
using OrganismStats::Muscle::MuscleQuality;
using OrganismStats::Skeleton::Bone;
using OrganismStats::Skeleton::SkeletonQuality;
using OrganismStats::Time::Age;
using OrganismStats::Time::FertilityCooldown;
using PhysicsStats::Density;
using PhysicsStats::Mass;
using PhysicsStats::SharedVolume;
using PhysicsStats::Volume;

struct Metabolism {
		Energy energy;
		Energy reserved;
		Diet   diet;

		[[nodiscard]]
		NormalizedValue<float> hungry() const;

		Metabolism(Energy energy, Energy reserved, Diet &&diet)
			: energy(energy)
			, reserved(reserved)
			, diet(diet) {}
};

struct Vital {
		Life     life;
		Immunity immunity;
		Health   health;
};

struct MuscleStructure {
		Muscle           muscle;
		MuscleEfficiency efficiency;
		MuscleQuality    quality;
		SharedVolume     shared_volume;
};

struct BodyReproductive {
		FertilityCooldown fertility;
		ReproductiveState state;
};

struct Morphology {
		Size size;
		Mass mass;

		[[nodiscard]]
		Mass density() const noexcept;

		[[nodiscard]]
		Volume volume() const noexcept;
};

struct SkeletonStructure {
		SharedVolume    shared_volume;
		Bone            bone;
		SkeletonQuality quality;
};

struct Body {
		Metabolism        metabolism;
		Vital             vital;
		BodyReproductive  reproductive;
		Morphology        morphology;
		SkeletonStructure skeleton;
		MuscleStructure   muscle;

		Age age;

		[[nodiscard]]
		Strength strength() const noexcept;

		[[nodiscard]]
		NormalizedValue<float> hungry() const noexcept;

		[[nodiscard]]
		float needed_energy() const noexcept;
};

struct Ontology {
		Gender      gender;
		std::string name;
		uint64_t    id;
};

struct Creature;

class Brain {
		friend class Creature;

	private:

		Intent intent_;

	public:

		Brain() {
			intent_.type = IntentTypes::NOTHING, intent_.time = 0;
		}

		std::optional<AttackedEvent> last_attack = std::nullopt;
		NormalizedValue<float>       sociability;

		const Intent &intent() const {
			return intent_;
		}

		void pass_intent() {
			intent_.time++;
		}

		void switch_intent(IntentTypes type) {
			intent_.type = type;
			intent_.time = 0;
		}
};

struct Creature {
		Genome genome;

		Brain    brain;
		Ontology ontology;
		Body     body;

		Vec3 position;

		Creature(Genome &&genome, Brain &&brain, Ontology &&ontology, Body &&body, Vec3 position)
			: genome(std::move(genome))
			, brain(std::move(brain))
			, ontology(std::move(ontology))
			, body(std::move(body))
			, position(position) {}

		Creature(const Genome &genome, Brain &&brain, Ontology &&ontology, Body &&body, Vec3 position)
			: genome(genome)
			, brain(std::move(brain))
			, ontology(std::move(ontology))
			, body(std::move(body))
			, position(position) {}

		Creature(const Creature &)            = delete;
		Creature &operator=(const Creature &) = delete;

		Creature(Creature &&)            = default;
		Creature &operator=(Creature &&) = default;

		[[nodiscard]] Id build_id() const noexcept {
			return IDF::create_creature_id(ontology.id);
		}

		[[nodiscard]]
		float specie_relative_muscle() const noexcept;

		[[nodiscard]]
		float specie_relative_bone() const noexcept;

		[[nodiscard]]
		float specie_relative_max_energy() const noexcept;

		[[nodiscard]]
		float specie_relative_max_reserved_energy() const noexcept;

		[[nodiscard]]
		float specie_relative_mass() const noexcept;

		[[nodiscard]]
		float specie_relative_volume() const noexcept;

		[[nodiscard]]
		float specie_relative_age() const noexcept;
};

using CreatureFunction = void (*)(Creature &);