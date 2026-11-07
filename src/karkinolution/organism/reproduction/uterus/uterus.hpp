#pragma once
#include <cstdint>
#include <shared/math/stats/runtime_values.hpp>
#include <karkinolution/organism/reproduction/gestation.hpp>
#include <karkinolution/organism/values.hpp>
#include <variant>
#include <vector>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Reproduction {
				namespace Uterus {
					using BornCount = IntegerLimited<int>;

					struct PregnantUterus {
							std::vector<uint64_t> embryos;
							Gestation             gestation;
							BornCount             born_count;

							int  embryos_count() const;
							int  dead_embryos_count() const;
							bool has_embryos() const;
							bool all_children_borned() const;
							bool is_full() const;
					};

					struct EmptyUterus {};

					using UterusState = std::variant<PregnantUterus, EmptyUterus>;

					class Uterus {
						public:

							UterusState state = EmptyUterus{};

							Uterus(const UterusState &state, const Health &health, const Energy &energy)
								: state(state)
								, health(health)
								, energy(energy) {}

							Health health;
							Energy energy;

							NormalizedValue<float> hungry() const;

							float pregnancy_cost() const;

							bool is_pregnant() const;
							bool is_empty() const;


							PregnantUterus       &get_pregnant_uterus();
							const PregnantUterus &get_pregnant_uterus() const;
					};

				} // namespace Uterus
			} // namespace Reproduction
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
