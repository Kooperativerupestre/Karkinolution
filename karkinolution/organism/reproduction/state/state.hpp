#pragma once
#include "karkinolution/organism/reproduction/state/ontology.hpp"
#include "karkinolution/organism/values.hpp"

#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/reproduction/gestation.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <utility>
#include <variant>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Reproduction {
				using ReproductiveOrgan = std::variant<std::monostate, Uterus::Uterus>;

				class ReproductiveState {
					public:

						ReproductiveOrgan                 state;
						State::Ontology::ReproductiveWays reproductive_way;

						bool is_pregnant() const {
							if (std::holds_alternative<std::monostate>(state)) {
								return false;
							} else {
								return (std::get<Uterus::Uterus>(state).is_pregnant());
							}
						}

						ReproductiveState(ReproductiveOrgan &&state, State::Ontology::ReproductiveWays way)
							: state(state)
							, reproductive_way(way) {}

						ReproductiveState(Creature::Gender                  gender,
										  State::Ontology::ReproductiveWays input_reproductive_way,
										  Energy                            initial_energy,
										  Health                            initial_health) {
							switch (gender) {
								case Creature::Gender::MALE:
									state = std::monostate();
									break;

								case Creature::Gender::FEMALE:
									state = Uterus::Uterus{Uterus::EmptyUterus{}, initial_health, initial_energy};
									break;

								default:
									std::unreachable();
							}

							reproductive_way = input_reproductive_way;
						}
				};

			} // namespace Reproduction
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
