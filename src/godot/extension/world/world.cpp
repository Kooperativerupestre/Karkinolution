#include "godot/extension/client/query/aliases.hpp"
#include "godot/extension/client/query/creature.hpp"
#include "godot/extension/model/aliases.hpp"
#include "karkinolution/core/id.hpp"

#include <godot/extension/world/world.hpp>

bool KarkinolutionGodot::World::GodotWorld::remove_corpse(BaseIdType id) {
	for (size_t i = 0; i < corpses.size(); i++) {
		godot::Ref<GodotCorpse> corpse = corpses[i];
		if (corpse->get_id() == id) {
			corpses.remove_at(id);
			return true;
		}
	}
	return false;
}

bool KarkinolutionGodot::World::GodotWorld::remove_creature(BaseIdType id) {
	for (size_t i = 0; i < creatures.size(); i++) {
		godot::Ref<GodotCreature> creature = creatures[i];

		if (creature->get_id() == id) {
			creatures.remove_at(id);
			return true;
		}
	}
	return false;
}

bool KarkinolutionGodot::World::GodotWorld::add(godot::Ref<GodotCreature> creature) {
	if (has_creature(creature->get_id())) {
		return false;
	}
	creatures.append(creature);
	return true;
}

bool KarkinolutionGodot::World::GodotWorld::add(godot::Ref<GodotCorpse> corpse) {
	if (has_corpse(corpse->get_id())) {
		return false;
	}
	corpses.append(corpse);
	return true;
}

bool KarkinolutionGodot::World::GodotWorld::update_creature(godot::Ref<GodotCreature> creature) {
	const auto id = creature->get_id();

	if (!has_creature(id)) {
		return false;
	}

	remove_creature(id);
	add(creature);
	return true;
}

bool KarkinolutionGodot::World::GodotWorld::update_corpse(godot::Ref<GodotCorpse> corpse) {
	const auto id = corpse->get_id();

	if (!has_corpse(id)) {
		return false;
	}

	remove_corpse(id);
	add(corpse);
	return true;
}

bool KarkinolutionGodot::World::GodotWorld::sync_creature(BaseIdType id) {
	if (!has_creature(id)) {
		return false;
	}

	const auto output = Query::Creature::see_creature(client, id);

	if (!output.is_valid()) {
		remove_creature(id);
		return false;
	}
	assert(update_creature(output) == true);
	return true;
}

bool KarkinolutionGodot::World::GodotWorld::sync_corpse(BaseIdType id) {
	if (!has_corpse(id)) {
		return false;
	}

	const auto output = Query::Corpse::see_corpse(client, id);
	if (!output.is_valid()) {
		remove_creature(id);
		return false;
	}
	assert(update_corpse(output) == true);
	return true;
}

bool KarkinolutionGodot::World::GodotWorld::has_creature(BaseIdType id) const {
	for (size_t i = 0; i < creatures.size(); i++) {
		godot::Ref<GodotCreature> creature = creatures[i];

		if (creature->get_id() == id) {
			return true;
		}
	}
	return false;
}

bool KarkinolutionGodot::World::GodotWorld::has_corpse(BaseIdType id) const {
	for (size_t i = 0; i < corpses.size(); i++) {
		godot::Ref<GodotCorpse> corpse = corpses[i];
		if (corpse->get_id() == id) {
			return true;
		}
	}

	return false;
}

bool KarkinolutionGodot::World::GodotWorld::has(BaseIdType id) const {
	if (has_creature(id)) {
		return true;
	}

	if (has_corpse(id)) {
		return true;
	}
	return false;
}

void KarkinolutionGodot::World::GodotWorld::sync() {
	for (size_t i = 0; i < creatures.size(); i++) {
		godot::Ref<GodotCreature> creature = creatures[i];

		sync_creature(creature->get_id());
	}

	for (size_t i = 0; i < corpses.size(); i++) {
		godot::Ref<GodotCorpse> corpse = corpses[i];
		sync_corpse(corpse->get_id());
	}
}

godot::TypedArray<GodotCreature> KarkinolutionGodot::World::GodotWorld::get_all_creatures() const {
	return creatures;
}

godot::TypedArray<GodotCorpse> KarkinolutionGodot::World::GodotWorld::get_all_corpses() const {
	return corpses;
}
