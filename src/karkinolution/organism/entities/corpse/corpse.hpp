#pragma once
#include <karkinolution/organism/stats.hpp>


using OrganismStats::Energy;


struct Corpse {
    uint64_t id;
    Energy energy;



    Corpse(const Corpse&) = delete;
    Corpse& operator=(const Corpse&) = delete;

    Corpse(Corpse&&) = default;
    Corpse& operator=(Corpse&&) = default;

    bool ready_to_disapear() const;
};

