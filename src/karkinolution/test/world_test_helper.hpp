#pragma once
#include "karkinolution/world/world.hpp"

namespace Karkinolution::World {

	// Note: Test helper class to provide tests with access to World's
	// private internal state (such as entity_map, territory, and entities_registry)
	// to verify internal invariant enforcement, state recovery, and atomicity.
	class WorldTestHelper {
		public:

			static Karkinolution::Terrain::EntityMap &get_entity_map(World &world) {
				return world.entity_map;
			}

			static const Karkinolution::Terrain::EntityMap &get_entity_map(const World &world) {
				return world.entity_map;
			}

			static Karkinolution::Terrain::Territory &get_territory(World &world) {
				return world.territory;
			}

			static const Karkinolution::Terrain::Territory &get_territory(const World &world) {
				return world.territory;
			}

			static Karkinolution::Organisms::Entities::EntitiesRegistry &get_entities_registry(World &world) {
				return world.entities_registry;
			}

			static const Karkinolution::Organisms::Entities::EntitiesRegistry &
			get_entities_registry(const World &world) {
				return world.entities_registry;
			}
	};

} // namespace Karkinolution::World

using Karkinolution::World::WorldTestHelper;
