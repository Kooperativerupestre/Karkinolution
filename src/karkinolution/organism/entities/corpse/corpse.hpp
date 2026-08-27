#pragma once
#include "karkinolution/math/physic/vec/model.hpp"
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/stats.hpp>

using OrganismStats::Energy;

struct Corpse {
    uint64_t id;
    RawMeat raw_meat;

    Vec3 position;

    Corpse(const Corpse&) = delete;
    Corpse& operator=(const Corpse&) = delete;

    Corpse(Corpse&&) = default;
    Corpse& operator=(Corpse&&) = default;

    Corpse(uint64_t id, RawMeat raw_meat) : id(id), raw_meat(raw_meat) {}

    bool ready_to_disapear() const;
};
