#include "karkinolution/organism/entities/corpse/corpse.hpp"

#include "karkinolution/organism/entities/corpse/motor.hpp"

#include <karkinolution/api/corpse.hpp>

Karkinolution::API::Corpse::CorpseReference
Karkinolution::API::Corpse::see_corpse(const Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id) {
	const auto entity_id = Karkinolution::Organisms::Entities::create_corpse_id(id);

	if (!world.see_entities_registry().exists(entity_id)) {
		return nullptr;
	}
	return std::get<Karkinolution::Organisms::Entities::Corpse::Corpse>(world.at_entity(entity_id));
}

std::vector<KShared::Core::Id::BaseIdType>
Karkinolution::API::Corpse::see_all_corpse_ids(const Karkinolution::World::World &world) {
	std::vector<KShared::Core::Id::BaseIdType> ids;
	for (const auto &id : world.see_entities_registry().keys()) {
		if (id.type == Karkinolution::Organisms::Entities::EntityTypes::CORPSE) {
			ids.push_back(id.value);
		}
	}
	return ids;
}

Karkinolution::API::Corpse::CorpseReference Karkinolution::API::Corpse::tick(Karkinolution::World::World  &world,
																			 KShared::Core::Id::BaseIdType id) {
	const auto entity_id = Karkinolution::Organisms::Entities::create_corpse_id(id);

	if (!world.see_entities_registry().exists(entity_id)) {
		return nullptr;
	}

	auto &corpse = std::get<Karkinolution::Organisms::Entities::Corpse::Corpse>(world.at_entity(entity_id));
	Karkinolution::Organisms::Entities::Corpse::Motor::run(corpse, world);

	return see_corpse(world, id);
}

bool Karkinolution::API::Corpse::erase(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id) {
	return world.remove(Karkinolution::Organisms::Entities::create_corpse_id(id));
}
