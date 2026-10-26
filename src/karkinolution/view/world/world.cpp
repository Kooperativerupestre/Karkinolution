#include "karkinolution/binary/interpreters/models/soil.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/world/queriers/creature.hpp"

#include <algorithm>
#include <karkinolution/view/world/queriers/corpse.hpp>
#include <karkinolution/view/world/queriers/soil.hpp>
#include <karkinolution/view/world/world.hpp>

// Terrain implementation

bool KarkinolutionView::World::Terrain::has_soil(BaseIdType id) const {
	for (size_t i = 0; i < soils.size(); i++) {
		if (soils[i].id == id) {
			return true;
		}
	}
	return false;
}

bool KarkinolutionView::World::Terrain::remove_soil(BaseIdType id) {
	const auto it =
		std::find_if(soils.begin(),
					 soils.end(),
					 [id](const Karkinolution::Binary::Interpreters::Models::Soil::Soil &soil) {
						 return soil.id == id;
					 });

	const bool removed = it == soils.end();

	if (removed) {
		soils.erase(it);
		return true;
	}
	return false;
}

bool KarkinolutionView::World::Terrain::add(
	const Karkinolution::Binary::Interpreters::Models::Soil::Soil &soil) {
	if (has_soil(soil.id)) {
		return false;
	}
	soils.push_back(soil);
	return true;
}

bool KarkinolutionView::World::Terrain::update(
	const Karkinolution::Binary::Interpreters::Models::Soil::Soil &new_soil) {
	if (!has_soil(new_soil.id)) {
		return false;
	}
	remove_soil(new_soil.id);
	add(new_soil);
	return true;
}

KarkinolutionView::World::EntitySyncOutput
KarkinolutionView::World::Terrain::sync_soil(BaseIdType                         id,
											 KarkinolutionView::Client::Client &client) {
	if (!has_soil(id)) {
		return EntitySyncOutput::DOES_NOT_EXIST;
	}

	const auto response = KarkinolutionView::World::Queriers::Soil::see_soil(id, client);
	if (!response) {
		remove_soil(id);
		return EntitySyncOutput::WAS_NOT_FOUND;
	}
	update(*response);
	return EntitySyncOutput::UPDATED;
}

void KarkinolutionView::World::Terrain::sync_size(KarkinolutionView::Client::Client &client) {}

void KarkinolutionView::World::Terrain::sync(KarkinolutionView::Client::Client &client) {
	std::vector<BaseIdType> soils_id_to_add;

	soils_id_to_add.reserve(soils.size());

	for (size_t i = 0; i < soils.size(); i++) {
		soils_id_to_add.push_back(soils[i].id);
	}
	for (size_t i = 0; i < soils_id_to_add.size(); i++) {
		sync_soil(soils_id_to_add[i], client);
	}
}

// World implementation


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

bool KarkinolutionView::World::World::add(
	const Karkinolution::Binary::Interpreters::Models::Creature::Creature &creature) {
	if (has_creature(creature.id)) {
		return false;
	}
	creatures.push_back(creature);
	return true;
}

bool KarkinolutionView::World::World::add(
	const Karkinolution::Binary::Interpreters::Models::Corpse::Corpse &corpse) {
	if (has_corpse(corpse.id)) {
		return false;
	}
	corpses.push_back(corpse);
	return true;
}

bool KarkinolutionView::World::World::remove_creature(BaseIdType id) {
	const auto it = std::find_if(
		creatures.begin(),
		creatures.end(),
		[id](const Karkinolution::Binary::Interpreters::Models::Creature::Creature &creature) {
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
	const auto it = std::find_if(
		corpses.begin(),
		corpses.end(),
		[id](const Karkinolution::Binary::Interpreters::Models::Corpse::Corpse &corpse) {
			return corpse.id == id;
		});

	const bool removed = it != corpses.end();
	if (removed) {
		corpses.erase(it);
		return true;
	}
	return false;
}

bool KarkinolutionView::World::World::update(
	const Karkinolution::Binary::Interpreters::Models::Creature::Creature &new_creature) {
	if (!has_creature(new_creature.id)) {
		return false;
	}

	assert(remove_creature(new_creature.id));
	assert(add(new_creature));
	return true;
}

bool KarkinolutionView::World::World::update(
	const Karkinolution::Binary::Interpreters::Models::Corpse::Corpse &new_corpse) {
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
	terrain.sync(client);

	std::vector<BaseIdType> creatures_id_to_add;
	std::vector<BaseIdType> corpses_id_to_add;
	creatures_id_to_add.reserve(creatures.size());
	corpses_id_to_add.reserve(corpses.size());

	for (size_t i = 0; i < creatures.size(); i++) {
		creatures_id_to_add.push_back(creatures[i].id);
	}
	for (size_t i = 0; i < corpses.size(); i++) {
		corpses_id_to_add.push_back(corpses[i].id);
	}

	for (size_t i = 0; i < creatures.size(); i++) {
		sync_creature(creatures_id_to_add[i], client);
	}
	for (size_t i = 0; i < corpses.size(); i++) {
		sync_corpse(corpses_id_to_add[i], client);
	}
}
