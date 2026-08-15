#pragma once
#include "karkinolution/math/units.hpp"
#include <karkinolution/math/stats/compile_values.hpp>
#include <cstdint>
#include <stdexcept>
#include <karkinolution/organism/foods/foods.hpp>
#include <cstdint>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/math/geometry/models.hpp>




struct AttackedEvent {
    Id attacker_id;
    float damage;
};

enum class Gender : uint8_t {
    MALE,
    FEMALE
};

namespace GenderF {
    Gender choice();
    Gender other_sex(Gender g);
}

class Diet {
    private:

    NormalizedValue<float> _grass_score;
    NormalizedValue<float> _raw_meat_score;

    public:

    const NormalizedValue<float>& grass_score() const;
    const NormalizedValue<float>& raw_meat_score() const;

    const NormalizedValue<float>& operator[](FoodHint food_hint) const;

    Diet(NormalizedValue<float> grass_score, NormalizedValue<float> raw_meat_score) {
        if ((grass_score + raw_meat_score).value() > 1.0f) {
            throw std::invalid_argument("Sum of scores > 1.0");
        }

        _grass_score = grass_score;
        _raw_meat_score = raw_meat_score;
    }
};

enum class Temperament : uint8_t {
    PASSIVE,
    NEUTRAL,
    AGGRESSIVE,
    TERRITORIAL
};

struct VisionRadius : public GeometryForms::Radius {
    using GeometryForms::Radius::Radius;
};

