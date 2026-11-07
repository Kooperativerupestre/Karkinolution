#pragma once

#include "karkinolution/organism/reproduction/state/ontology.hpp"
#include "shared/core/id.hpp"
#include "shared/math/physic/vec/model.hpp"

#include <cstdint>
#include <karkinolution/organism/entities/creature/brain/intents.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/internal_aliases.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/organism/values.hpp>
#include <shared/math/stats/compile_values.hpp>
#include <shared/math/units.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Creature {


				struct MetabolismPart {
						Energy energy;
						Energy reserved;
						Diet   diet;

						[[nodiscard]]
						NormalizedValue<float> hungry() const;

						MetabolismPart(Energy energy, Energy reserved, Diet &&diet)
							: energy(energy)
							, reserved(reserved)
							, diet(diet) {}
				};

				struct VitalPart {
						Life     life;
						Immunity immunity;
						Health   health;
				};

				struct MorphologyPart {
						Size size;
						Mass mass;

						[[nodiscard]]
						Mass density() const noexcept;

						[[nodiscard]]
						Area area() const noexcept;
				};

				struct ReproductivePart {
						FertilityCooldown               fertility;
						Reproduction::ReproductiveState state;
				};

				struct Body {
						MetabolismPart   metabolism;
						VitalPart        vital;
						MorphologyPart   morphology;
						ReproductivePart reproduction;

						Age age;

						[[nodiscard]]
						NormalizedValue<float> hungry() const noexcept;

						[[nodiscard]]
						float needed_energy() const noexcept;
				};

				struct Ontology {
						Gender      gender;
						std::string name;
						BaseIdType  id;
				};

				struct Creature;

				namespace Brain {
					class Brain {
							friend class Creature;

						private:

							Intents::Intent intent_;

						public:

							Brain() {
								intent_.type = Intents::IntentTypes::NOTHING, intent_.time = 0;
							}

							std::optional<AttackedEvent> last_attack = std::nullopt;
							NormalizedValue<float>       sociability;

							const Intents::Intent &intent() const {
								return intent_;
							}

							void pass_intent() {
								intent_.time++;
							}

							void switch_intent(Intents::IntentTypes type) {
								intent_.type = type;
								intent_.time = 0;
							}
					};
				} // namespace Brain

				struct Creature {
						Genetic::Genome genome;

						Brain::Brain brain;
						Ontology     ontology;
						Body         body;

						Vec2 position;

						[[nodiscard]]
						Strength strength() const noexcept;

						Creature(Genetic::Genome &&genome,
								 Brain::Brain    &&brain,
								 Ontology        &&ontology,
								 Body            &&body,
								 Vec2              position)
							: genome(std::move(genome))
							, brain(brain)
							, ontology(std::move(ontology))
							, body(std::move(body))
							, position(position) {}

						Creature(const Genetic::Genome &genome,
								 Brain::Brain         &&brain,
								 Ontology             &&ontology,
								 Body                 &&body,
								 Vec2                   position)
							: genome(genome)
							, brain(brain)
							, ontology(std::move(ontology))
							, body(std::move(body))
							, position(position) {}

						Creature(const Creature &)            = delete;
						Creature &operator=(const Creature &) = delete;

						Creature(Creature &&)            = default;
						Creature &operator=(Creature &&) = default;

						[[nodiscard]] EntityId build_id() const noexcept {
							return create_creature_id(ontology.id);
						}

						[[nodiscard]]
						float specie_relative_max_energy() const noexcept;

						[[nodiscard]]
						float specie_relative_max_reserved_energy() const noexcept;

						[[nodiscard]]
						float specie_relative_mass() const noexcept;

						[[nodiscard]]
						float specie_relative_area() const noexcept;

						[[nodiscard]]
						float specie_relative_age() const noexcept;
				};
			} // namespace Creature
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution