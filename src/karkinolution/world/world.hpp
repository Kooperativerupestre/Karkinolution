#pragma once
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/organism/genetics.hpp>
#include <karkinolution/terrain/map.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/systems/reproductivebuffer.hpp>
#include <string>

struct LogEntry {
    int time;
    std::string message;
};

class Log : public BaseStorage<int, LogEntry> {
private:
    int next_id = 0;

public:
    void add(LogEntry entry) {
        BaseStorage<int, LogEntry>::add(next_id++, entry);
    }
};

struct World {
    Territory territory;
    EntityMap entity_map;
    EntitiesRegistry entities;
    ReproductiveBuffer reproductive_buffer;
    Log log;
    int time = 0;

    void add_log(const std::string& message) {
        log.add(LogEntry{time, message});
    }
};

namespace WorldMotor {
    void add_entity(World&world, Entity&&entity);
    void add_random_creatures(World&world, int k);
    void add_specie_random_creature(World&world,int k, CreatureSpecies specie);
    void delete_entity(World&world, Id id);
}

struct PresetWorld {
    int seed;
    Vec2 size;
    float scale;
};

namespace WorldFactory {
    World create_world(PresetWorld preset);
    World create_crab_chaos();
    World create_paranoic();
    World create_titanic();
    World create_normal();
}

