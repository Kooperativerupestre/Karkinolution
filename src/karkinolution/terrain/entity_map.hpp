#pragma once
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/terrain/terrain.hpp"
#include "shared/math/geometry/models.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/egg/egg.hpp>
#include <shared/functional/quadtree.hpp>
#include <shared/math/geometry/models.hpp>
#include <shared/math/physic/vec/model.hpp>
#include <shared/math/units.hpp>

namespace Karkinolution::Terrain {

	class EntityMap {
		private:

			KShared::Functional::Quadtree::Quadtree<Karkinolution::Organisms::Entities::EntityId> quadtree;


		public:

			EntityMap(const KShared::Math::Units::Size &size);

			bool add(Karkinolution::Organisms::Entities::EntityId id,
					 const KShared::Math::Vec::Vec2              &position,
					 const KShared::Math::Units::Size            &size,
					 const Karkinolution::Terrain::Territory     &territory);

			bool remove(Karkinolution::Organisms::Entities::EntityId id,
						const KShared::Math::Units::Size            &old_size,
						const KShared::Math::Vec::Vec2              &old_position);
			bool remove(Karkinolution::Organisms::Entities::EntityId id);

			std::vector<Karkinolution::Organisms::Entities::EntityId>
			find(const KShared::Math::Units::Size &size, const KShared::Math::Vec::Vec2 &position) const;

			bool has_entity(Karkinolution::Organisms::Entities::EntityId id) const;

			std::optional<Karkinolution::Organisms::Entities::EntityId>
			if_find_one_stop(const KShared::Math::Units::Size &size, const KShared::Math::Vec::Vec2 &position) const;
			bool update_coord(Karkinolution::Organisms::Entities::EntityId id,
							  const KShared::Math::Units::Size            &current_size,
							  const KShared::Math::Vec::Vec2              &current_position,
							  const KShared::Math::Vec::Vec2              &new_position,
							  const Karkinolution::Terrain::Territory     &territory);

			bool can_contain(const KShared::Math::Units::Size &size, const KShared::Math::Vec::Vec2 &position) const;
	};

} // namespace Karkinolution::Terrain
