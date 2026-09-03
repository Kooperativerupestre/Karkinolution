#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <stdexcept>

// Diet

const NormalizedValue<float> &Diet::grass_score() const {
	return _grass_score;
}

const NormalizedValue<float> &Diet::raw_meat_score() const {
	return _raw_meat_score;
}

const NormalizedValue<float> &Diet::operator[](FoodHint food_hint) const {
	if (food_hint == FoodHint::GRASS) {
		return _grass_score;
	} else if (food_hint == FoodHint::RAW_MEAT) {
		return _raw_meat_score;
	} else {
		throw std::runtime_error("Enum doesn't exist");
	}
}

// Gender

Gender GenderF::other_sex(Gender g) {
	if (g == Gender::MALE) {
		return Gender::FEMALE;
	} else {
		return Gender::MALE;
	}
}

Gender GenderF::choice() {
	return Choices::choice({Gender::MALE, Gender::FEMALE});
}