#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/identity.hpp"

#include <karkinolution/api/creature.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/world.hpp>

const Creature* CreatureAPI::get_creature(const World &world, BaseIdType id) {
	const auto creature =
		world.organism_registry.entities.try_at(EntityIDF::create_creature_id(id));

	if (creature == nullptr) {
		return nullptr;
	}
	return &std::get<Creature>(*creature);
}
