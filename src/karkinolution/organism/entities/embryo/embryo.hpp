#pragma once
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/math/units.hpp>

using OrganismStats::Energy;
using OrganismStats::Life;
using OrganismStats::Health::Health;


class EmbryonicEnergy : public FloatValue{
    public:

    using FloatValue::FloatValue;
};

class EmbryoHungry : public LimitedValue<EmbryoHungry, float, 2.0f, 0.0f> {
    public:

    template <typename T>
    EmbryoHungry(T value) : LimitedValue(value) {}

    EmbryoHungry() : LimitedValue(0) {}
};

struct Embryo {
    uint64_t id;
    Energy energy;
    Life life;
    Health health;
    Size volume;
    Genome genome;
    int age;

    NormalizedValue<float> remaining_growth_capacity() const;
    EmbryoHungry hungry() const;
};

using EmbryoFunction = void (*)(Embryo&);