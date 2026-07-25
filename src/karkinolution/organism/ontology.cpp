#include "karkinolution/organism/ontology.hpp"
#include <algorithm>
#include <cstdint>
#include <random>
#include <unordered_map>

#include "karkinolution/utils/k_random.hpp"
#include <limits>

// Diet
std::unordered_map<FoodHint, float> Diet::normalize(float corpse_score, float grass_score, float target_score) {
    float max_score = std::max({corpse_score, grass_score, target_score});
    return {
        {FoodHint::CORPSE, corpse_score},
        {FoodHint::GRASS, grass_score},
        {FoodHint::TARGET, target_score}
    };
}

Diet::Diet(float corpse_score, float grass_score, float target_score) : diet(normalize(corpse_score, grass_score, target_score)) {}

float Diet::operator[](FoodHint food_hint) const {
    return diet.at(food_hint);
}

Diet Diet::scramble(const Diet&other_diet) const {
    return Diet{
        Disturbs::mini_scramble(other_diet[FoodHint::CORPSE], (*this)[FoodHint::CORPSE]),
        Disturbs::mini_scramble(other_diet[FoodHint::GRASS], (*this)[FoodHint::GRASS]),
        Disturbs::mini_scramble(other_diet[FoodHint::TARGET], (*this)[FoodHint::TARGET])
    };
}

// Id

uint64_t IDF::gen_id() {
    std::uniform_int_distribution<uint64_t> dist(
        std::numeric_limits<uint64_t>::min(),
        std::numeric_limits<uint64_t>::max()
    );
    return dist(gen);
}

Id IDF::create_creature_id(uint64_t id) {
    return Id{id, EntityTypes::CREATURE};
}

Id IDF::create_corpse_id(uint64_t id) {
    return Id{id, EntityTypes::CORPSE};
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