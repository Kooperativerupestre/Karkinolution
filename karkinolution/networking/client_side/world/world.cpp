#include "karkinolution/networking/client_side/world/world.hpp"

#include "karkinolution/networking/client_side/world/aliases.hpp"

#include <algorithm>
#include <cassert>

namespace Karkinolution::Networking::ClientSide::World {
	///
	/// Territory
	///
	bool Territory::has_soil(SoilId id) const {
		for (size_t i = 0; i < soils.size(); i++) {
			if (soils[i].id == id) {
				return true;
			}
		}
		return false;
	}

	bool Territory::remove_soil(SoilId id) {
		const auto it = std::find_if(soils.begin(), soils.end(), [id](const Soil &soil) {
			return soil.id == id;
		});

		if (it != soils.end()) {
			soils.erase(it);
			return true;
		}
		return false;
	}

	bool Territory::add(const Soil &soil) {
		if (has_soil(soil.id)) {
			return false;
		}
		soils.push_back(soil);
		return true;
	}

	bool Territory::update(const Soil &soil) {
		if (!has_soil(soil.id)) {
			return false;
		}
		assert(remove_soil(soil.id));
		return add(soil);
	}

	///
	/// ENTITIES STORAGE
	///
	bool EntitiesStorage::has_creature(OrganismId id) const {
		for (size_t i = 0; i < creatures.size(); i++) {
			if (creatures[i].id == id) {
				return true;
			}
		}
		return false;
	}

	bool EntitiesStorage::has_corpse(OrganismId id) const {
		for (size_t i = 0; i < corpses.size(); i++) {
			if (corpses[i].id == id) {
				return true;
			}
		}
		return false;
	}

	bool EntitiesStorage::has(OrganismId id) const {
		return has_creature(id) || has_corpse(id);
	}

	bool EntitiesStorage::remove_creature(OrganismId id) {
		const auto it = std::find_if(creatures.begin(), creatures.end(), [id](const Creature &creature) {
			return creature.id == id;
		});

		if (it != creatures.end()) {
			creatures.erase(it);
			return true;
		}
		return false;
	}

	bool EntitiesStorage::remove_corpse(OrganismId id) {
		const auto it = std::find_if(corpses.begin(), corpses.end(), [id](const Corpse &corpse) {
			return corpse.id == id;
		});

		if (it != corpses.end()) {
			corpses.erase(it);
			return true;
		}
		return false;
	}

	bool EntitiesStorage::add(const Creature &creature) {
		if (has_creature(creature.id)) {
			return false;
		}
		creatures.push_back(creature);
		return true;
	}

	bool EntitiesStorage::add(const Corpse &corpse) {
		if (has_corpse(corpse.id)) {
			return false;
		}
		corpses.push_back(corpse);
		return true;
	}

	bool EntitiesStorage::update(const Creature &creature) {
		if (!has_creature(creature.id)) {
			return false;
		}
		assert(remove_creature(creature.id));
		return add(creature);
	}

	bool EntitiesStorage::update(const Corpse &corpse) {
		if (!has_corpse(corpse.id)) {
			return false;
		}
		assert(remove_corpse(corpse.id));
		return add(corpse);
	}

	///
	/// WORLD
	///
	bool World::add(const Soil &soil) {
		return territory.add(soil);
	}

	bool World::add(const Creature &creature) {
		return entities.add(creature);
	}

	bool World::add(const Corpse &corpse) {
		return entities.add(corpse);
	}

	bool World::has_creature(OrganismId id) const {
		return entities.has_creature(id);
	}

	bool World::has_corpse(OrganismId id) const {
		return entities.has_corpse(id);
	}

	bool World::has_soil(SoilId id) const {
		return territory.has_soil(id);
	}
} // namespace Karkinolution::Networking::ClientSide::World
