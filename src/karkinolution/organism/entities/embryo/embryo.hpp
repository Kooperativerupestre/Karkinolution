#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/identity.hpp"

#include <cmath>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Embryo {
				class EmbryonicEnergy : public FloatValue {
					public:

						using FloatValue::FloatValue;
				};

				class EmbryoHungry : public LimitedValue<EmbryoHungry, float, 2.0f, 0.0f> {
					public:

						template <typename T>
						EmbryoHungry(T value)
							: LimitedValue(value) {}

						EmbryoHungry()
							: LimitedValue(0) {}
				};

				struct Embryo {
						BaseIdType      id;
						Energy          energy;
						Life            life;
						Health          health;
						Area            area;
						Genetic::Genome genome;
						int             age;
						Vec2            position;

						[[nodiscard]] Size size() const noexcept {
							const double width = std::sqrt(area.value * 1.6);
							return Size{.width = Width{width}, .height = Height{area.value / width}};
						}

						[[nodiscard]] EntityId build_id() const noexcept {
							return create_embryo_id(id);
						}

						[[nodiscard]] EmbryoHungry hungry() const;

						[[nodiscard]] double specie_relative_area() const;
						[[nodiscard]] float  specie_relative_energy_max() const;
						[[nodiscard]] float  specie_relative_life() const;
				};

				using EmbryoFunction = void (*)(Embryo &);

			} // namespace Embryo
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
