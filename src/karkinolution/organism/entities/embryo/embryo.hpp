#pragma once
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/math/units.hpp>

using OrganismStats::Energy;
using OrganismStats::Life;
using OrganismStats::Health::Health;
using PhysicsStats::Volume;

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
    Volume volume;
    Genome genome;
    int age;

    [[nodiscard]] EmbryoHungry hungry() const;


    [[nodiscard]] float specie_relative_volume() const;
    [[nodiscard]] float specie_relative_energy_max() const;
    [[nodiscard]] float specie_relative_life() const;
    
    
};

using EmbryoFunction = void (*)(Embryo&);