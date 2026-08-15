#pragma once
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/foods/nutrient.hpp>

struct Trade {
    CreatureFunction gain;
    FoodFunction loss;
};

namespace CreatureGrowingPhysiology {
    float get_new_max_energy_increment(const Creature&creature, const EntitiesRegistry&entities);
    float get_new_max_energy_reserved_increment(const Creature&creature, const EntitiesRegistry&entities);
    float get_new_max_life_increment(const Creature&creature, const EntitiesRegistry&entities);
    float get_new_health_increment(const Creature&creature, const EntitiesRegistry&entities);

    float get_new_lateral_increment(const Creature&creature, const EntitiesRegistry&entities);
    float get_new_back_increment(const Creature&creature, const EntitiesRegistry&entities);
    float get_new_height_increment(const Creature&creature, const EntitiesRegistry&entities);
    float get_new_muscle_increment(const Creature&creature, const EntitiesRegistry&entities);
    float get_new_skeleton_increment(const Creature&creature, const EntitiesRegistry&entities);
}

namespace CreatureMetabolismPhysiology {
    Trade metabolize(const Creature&creature, const Grass&raw_meat, const EntitiesRegistry&entities);
    Trade metabolize(const Creature&creature, const RawMeat&raw_meat, const EntitiesRegistry&entites);
}

namespace CreaturePhysiology {   
    CreatureFunction get_to_age_effects(Creature&creature, int age = 1);
    CreatureFunction get_diseases_effect(const Creature&creature, const EntitiesRegistry&entities);
    bool is_dead(const Creature&creature);
}

