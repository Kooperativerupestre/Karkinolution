#include "karkinolution/binary/interpreters/corpse.hpp"
#include "karkinolution/binary/interpreters/creature.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/world/queriers/creature.hpp"

#include <algorithm>
#include <karkinolution/view/world/queriers/corpse.hpp>
#include <karkinolution/view/world/world.hpp>

bool KarkinolutionView::World::World::has_creature(BaseIdType id) const {
	for (size_t i = 0; i < creatures.size(); i++) {
		if (creatures[i].id == id) {
			return true;
		}
	}
	return false;
}

bool KarkinolutionView::World::World::has_corpse(BaseIdType id) const {
	for (size_t i = 0; i < corpses.size(); i++) {
		if (corpses[i].id == id) {
			return true;
		}
	}
	return false;
}

bool KarkinolutionView::World::World::has(BaseIdType id) const {
	if (has_creature(id)) {
		return true;
	}
	if (has_corpse(id)) {
		return true;
	}
	return false;
}

bool KarkinolutionView::World::World::add(const CreatureDSI::Models::Creature &creature) {
	if (has_creature(creature.id)) {
		return false;
	}
	creatures.push_back(creature);
	return true;
}

bool KarkinolutionView::World::World::add(const CorpseDSI::Models::Corpse &corpse) {
	if (has_corpse(corpse.id)) {
		return false;
	}
	corpses.push_back(corpse);
	return true;
}

bool KarkinolutionView::World::World::remove_creature(BaseIdType id) {
	const auto it = std::find_if(creatures.begin(),
								 creatures.end(),
								 [id](const CreatureDSI::Models::Creature &creature) {
									 return creature.id == id;
								 });

	const bool removed = it != creatures.end();

	if (removed) {
		creatures.erase(it);
		return true;
	}
	return false;
}

bool KarkinolutionView::World::World::remove_corpse(BaseIdType id) {
	const auto it =
		std::find_if(corpses.begin(), corpses.end(), [id](const CorpseDSI::Models::Corpse &corpse) {
			return corpse.id == id;
		});

	const bool removed = it != corpses.end();
	if (removed) {
		corpses.erase(it);
		return true;
	}
	return false;
}

bool KarkinolutionView::World::World::update(const CreatureDSI::Models::Creature &new_creature) {
	if (!has_creature(new_creature.id)) {
		return false;
	}

	assert(remove_creature(new_creature.id));
	assert(add(new_creature));
	return true;
}

bool KarkinolutionView::World::World::update(const CorpseDSI::Models::Corpse &new_corpse) {
	if (!has_corpse(new_corpse.id)) {
		return false;
	}
	assert(remove_corpse(new_corpse.id));

	assert(add(new_corpse));
	return true;
}

KarkinolutionView::World::EntitySyncOutput
KarkinolutionView::World::World::sync_creature(BaseIdType                         id,
											   KarkinolutionView::Client::Client &client) {
	if (!has_creature(id)) {
		return EntitySyncOutput::DOES_NOT_EXIST;
	}
	const auto response = KarkinolutionView::World::Queriers::Creature::see_creature(id, client);
	if (!response) {
		remove_creature(id);
		return EntitySyncOutput::WAS_NOT_FOUND;
	}
	update(*response);
	return EntitySyncOutput::UPDATED;
}

KarkinolutionView::World::EntitySyncOutput
KarkinolutionView::World::World::sync_corpse(BaseIdType                         id,
											 KarkinolutionView::Client::Client &client) {
	if (!has_corpse(id)) {
		return EntitySyncOutput::DOES_NOT_EXIST;
	}
	const auto response = KarkinolutionView::World::Queriers::Corpse::see_corpse(id, client);
	if (!response) {
		remove_corpse(id);
		return EntitySyncOutput::WAS_NOT_FOUND;
	}
	update(*response);
	return EntitySyncOutput::UPDATED;
}

void KarkinolutionView::World::World::sync(KarkinolutionView::Client::Client &client) {
	std::vector<BaseIdType> creatures_id_to_add;
	std::vector<BaseIdType> corpses_id_to_add;
	creatures_id_to_add.reserve(creatures.size());
	corpses_id_to_add.reserve(corpses.size());

	for (size_t i = 0; i < creatures.size(); i++) {
		sync_creature(creatures_id_to_add[i], client);
	}
	for (size_t i = 0; i < corpses.size(); i++) {
		sync_corpse(corpses_id_to_add[i], client);
	}
}
