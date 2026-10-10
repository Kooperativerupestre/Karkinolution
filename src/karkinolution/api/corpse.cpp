#include "karkinolution/organism/entities/corpse/corpse.hpp"

#include <karkinolution/api/corpse.hpp>

const Corpse* Karkinolution::API::Organisms::Corpse::see_corpse(const World &world, BaseIdType id) {
	const auto corpse = world.organism_registry.entities.try_at(EntityIDF::create_corpse_id(id));

	if (corpse == nullptr) {
		return nullptr;
	}
	return &std::get<::Corpse>(*corpse);
}