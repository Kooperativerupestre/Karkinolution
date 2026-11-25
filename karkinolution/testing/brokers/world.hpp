#pragma once

#include "karkinolution/world/world.hpp"

namespace Karkinolution::Testing::Brokers::World {

	class Access {
		public:

			static Karkinolution::Terrain::EntityMap &get_entity_map(Karkinolution::World::World &world) {
				return world.entity_map;
			}

			static const Karkinolution::Terrain::EntityMap &get_entity_map(const Karkinolution::World::World &world) {
				return world.entity_map;
			}

			static Karkinolution::Terrain::Territory &get_territory(Karkinolution::World::World &world) {
				return world.territory;
			}

			static const Karkinolution::Terrain::Territory &get_territory(const Karkinolution::World::World &world) {
				return world.territory;
			}

			static Karkinolution::Organisms::Entities::EntitiesRegistry &
			get_entities_registry(Karkinolution::World::World &world) {
				return world.entities_registry;
			}

			static const Karkinolution::Organisms::Entities::EntitiesRegistry &
			get_entities_registry(const Karkinolution::World::World &world) {
				return world.entities_registry;
			}
	};

} // namespace Karkinolution::Testing::Brokers::World
