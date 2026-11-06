

#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/internal_aliases.hpp>
#include <stdexcept>

namespace Karkinolution::Organisms::Entities::Creature {

	// Diet

	const NormalizedValue<float> &Diet::grass_matter_score() const {
		return _grass_matter_score;
	}

	const NormalizedValue<float> &Diet::raw_meat_score() const {
		return _raw_meat_score;
	}

	const NormalizedValue<float> &Diet::operator[](FoodHint food_hint) const {
		if (food_hint == FoodHint::GRASS_MATTER) {
			return _grass_matter_score;
		} else if (food_hint == FoodHint::RAW_MEAT) {
			return _raw_meat_score;
		} else {
			throw std::runtime_error("Enum doesn't exist");
		}
	}

	// Gender

	Gender inverse_sex(Gender g) {
		if (g == Gender::MALE) {
			return Gender::FEMALE;
		} else {
			return Gender::MALE;
		}
	}

	Gender choose_random_gender() {
		return KRandom::choice(std::vector<Gender>{Gender::FEMALE, Gender::FEMALE});
	}

	CreatureSpecies chooes_random_specie() {
		return KRandom::choice(
			{CreatureSpecies::CRAB, CreatureSpecies::CROCODILE, CreatureSpecies::FISH, CreatureSpecies::HIPPOPOTAMUS});
	}

} // namespace Karkinolution::Organisms::Entities::Creature