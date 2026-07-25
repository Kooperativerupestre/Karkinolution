#include <cstdint>
#include <karkinolution/world/world.hpp>
#include <karkinolution/ticks/worldcycle.hpp>
#include <iostream>

int main() {
    World world = WorldFactory::create_crab_chaos();
    std::cout << "Creatures in world: " << world.entities.size() << std::endl;

    for (uint8_t i = 0; i < 16; i++) {
        std::cout << "Running tick " << (int)i << std::endl;
        RunnerWorld::run(world);
    }
    std::cout << "Size of log: " << world.log.size() << std::endl;
    for (auto & log : world.log.values()) {
        std::cout << log.time  << ": " << log.message << std::endl;
    }

    std::cout << "World ended" << std::endl;
}