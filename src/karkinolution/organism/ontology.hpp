#pragma once
#include <cstdint>
#include <unordered_map>
#include <cstdint>


enum class EntityTypes : uint8_t {
    CREATURE,
    CORPSE
};


struct Id {
    uint64_t value;
    EntityTypes entity_type;

    bool operator==(const Id&) const = default;
};

template <>
struct std::hash<Id> {
    std::size_t operator()(const Id& id) const noexcept {
        std::size_t h1 = std::hash<uint64_t>{}(id.value);
        std::size_t h2 = std::hash<EntityTypes>{}(id.entity_type);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

namespace IDF {

    uint64_t gen_id();
    Id create_creature_id(uint64_t id);

    Id create_corpse_id(uint64_t id);
}



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

enum class FoodHint : uint8_t {
    CORPSE,
    TARGET,
    GRASS
};

class Diet {
    public:


    Diet(float corpse_score, float grass_score, float target_score);
    Diet scramble(const Diet&other_diet) const;
    float operator[](FoodHint food_hint) const;
    static std::unordered_map<FoodHint, float> normalize(float corpse_score, float grass_score, float target_score);
    
    private:

    std::unordered_map<FoodHint, float> diet;
};

enum class Temperament : uint8_t {
    PASSIVE,
    NEUTRAL,
    AGGRESSIVE,
    TERRITORIAL
};

