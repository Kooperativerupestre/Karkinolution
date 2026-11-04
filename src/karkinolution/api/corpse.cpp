#include "karkinolution/organism/entities/corpse/corpse.hpp"

#include <karkinolution/api/corpse.hpp>

const Karkinolution::Organisms::Entities::Corpse::Corpse*
Karkinolution::API::Corpse::see_corpse(const Karkinolution::World::World  &world,
									   Karkinolution::Core::Id::BaseIdType id) {
	const auto entity_id = Karkinolution::Organisms::Entities::create_corpse_id(id);

	if (!world.see_entities_registry().exists(entity_id)) {
		return nullptr;
	}
	return &std::get<Karkinolution::Organisms::Entities::Corpse::Corpse>(
		world.at_entity(entity_id));
}