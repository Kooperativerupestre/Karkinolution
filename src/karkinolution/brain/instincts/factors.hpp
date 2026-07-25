#pragma once
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/core/error.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <unordered_map>

inline const std::unordered_map<Temperament, float> COURAGE_FACTOR {
    {Temperament::PASSIVE, 0.1f},
    {Temperament::AGGRESSIVE, 0.7f},
    {Temperament::TERRITORIAL, 1.0f},
    {Temperament::NEUTRAL, 0.3f}
};


inline const std::unordered_map<Temperament, float> TRADE_OFF = {
    {Temperament::PASSIVE, 0.1f},
    {Temperament::NEUTRAL, 0.5f},
    {Temperament::AGGRESSIVE, 0.89f},
    {Temperament::TERRITORIAL, 1.0f}
};

namespace Factors {
    NormalizedValue get_pregnancy_risk(const Creature&creature);
    NormalizedValue get_fear(const Creature&creature);
    NormalizedValue get_courage(const Creature&creature);
    NormalizedValue get_territorial_indifference_factor(const Creature&creature);
    NormalizedValue get_pregnant_social_interest_factor(const Creature&creature);
    NormalizedValue get_pregnant_territorial_avoidance_attack_factor(const Creature&creature);
}

