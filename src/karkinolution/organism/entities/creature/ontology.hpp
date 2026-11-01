#pragma once
#include "karkinolution/math/units.hpp"

#include <cstdint>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <stdexcept>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Creature {

				struct AttackedEvent {
						EntityId attacker_id;
						float    damage;
				};

				enum class Gender : uint8_t {
					MALE,
					FEMALE
				};
				enum class CreatureSpecies : uint8_t {
					CRAB,
					FISH,
					CROCODILE,
					HIPPOPOTAMUS
				};

				CreatureSpecies chooes_random_specie();
				Gender          choose_random_gender();
				Gender          inverse_sex(Gender g);

				class Diet {
					private:

						NormalizedValue<float> _grass_matter_score;
						NormalizedValue<float> _raw_meat_score;

					public:

						const NormalizedValue<float> &grass_matter_score() const;
						const NormalizedValue<float> &raw_meat_score() const;

						const NormalizedValue<float> &operator[](FoodHint food_hint) const;

						Diet(NormalizedValue<float> grass_score,
							 NormalizedValue<float> raw_meat_score) {
							if ((grass_score + raw_meat_score).value() > 1.0f) {
								throw std::invalid_argument("Sum of scores > 1.0");
							}

							_grass_matter_score = grass_score;
							_raw_meat_score     = raw_meat_score;
						}
				};

				enum class Temperament : uint8_t {
					PASSIVE,
					NEUTRAL,
					AGGRESSIVE,
					TERRITORIAL
				};

				struct VisionRadius : public Karkinolution::Math::Geometry::Radius {
						using Karkinolution::Math::Geometry::Radius::Radius;
				};

			} // namespace Creature
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
