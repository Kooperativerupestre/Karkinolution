#include <karkinolution/world/motor.hpp>
#include <karkinolution/world/world.hpp>

void WorldMotor::remove(Id id, World &world) {
	world.organism_registry.entities.del(id);
	world.entity_map.root().remove(id);
}
