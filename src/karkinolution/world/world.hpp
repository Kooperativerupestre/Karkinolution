#pragma once

#include "shared/math/physic/vec/model.hpp"
#include "shared/math/units.hpp"
#include "karkinolution/organism/entities/creature/creature.hpp"
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/terrain/natures_map.hpp"

#include <karkinolution/terrain/entity_map.hpp>
#include <karkinolution/terrain/natures_map.hpp>
#include <karkinolution/terrain/terrain.hpp>

namespace Karkinolution {
	namespace World {

		class WorldTestHelper;

		struct WorldPreset {
				int                              seed;
				KShared::Math::Units::Size size;
				float                            epsilon;
		};

		class World {
				// Note: Friend class for testing purposes, enabling test suites to inspect
				// internal components and verify invariant enforcement and atomicity.
				friend class WorldTestHelper;

			private:

				Karkinolution::Terrain::EntityMap                    entity_map;
				Karkinolution::Organisms::Entities::EntitiesRegistry entities_registry;
				Karkinolution::Terrain::NaturesMap                   natures;
				Karkinolution::Terrain::Territory                    territory;
				int                                                  time = 0;


			public:

				const Karkinolution::Terrain::EntityMap &see_entity_map() const {
					return entity_map;
				}

				const Karkinolution::Organisms::Entities::EntitiesRegistry &see_entities_registry() const {
					return entities_registry;
				}

				const Karkinolution::Terrain::NaturesMap &see_natures() const {
					return natures;
				}

				const Karkinolution::Terrain::Territory &see_territory() const {
					return territory;
				}

				Karkinolution::Terrain::Territory &see_territory() {
					return territory;
				}

				void pass_time() {
					++time;
				}

				World(const WorldPreset &preset);

				bool remove(Karkinolution::Organisms::Entities::EntityId id);
				bool add(Karkinolution::Organisms::Entities::Entity &&entity);
				bool update(Karkinolution::Organisms::Entities::EntityId id,
							const KShared::Math::Vec::Vec2        &new_position);

				bool exists_entity(Karkinolution::Organisms::Entities::EntityId id) const {
					return entities_registry.exists(id) && entity_map.has_entity(id);
				}

				Karkinolution::Organisms::Entities::Entity &at_entity(Karkinolution::Organisms::Entities::EntityId id) {
					return entities_registry.at(id);
				}

				const Karkinolution::Organisms::Entities::Entity &
				at_entity(Karkinolution::Organisms::Entities::EntityId id) const {
					return entities_registry.at(id);
				}

				Karkinolution::Organisms::Entities::Creature::Creature &
				at_creature(Karkinolution::Organisms::Entities::EntityId id) {
					return entities_registry.at_creature(id);
				}

				const Karkinolution::Organisms::Entities::Creature::Creature &
				at_creature(Karkinolution::Organisms::Entities::EntityId id) const {
					return entities_registry.at_creature(id);
				}
		};

	} // namespace World
} // namespace Karkinolution
