#pragma once
#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/terrain/terrain.hpp"

#include <karkinolution/functional/quadtree.hpp>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/egg/egg.hpp>

namespace Karkinolution::Terrain {

	class EntityMap {
		private:

			Karkinolution::Functional::Quadtree::Quadtree<
				Karkinolution::Organisms::Entities::EntityId>
				quadtree;


		public:

			EntityMap(const Karkinolution::Math::Units::Size &size);

			bool add(Karkinolution::Organisms::Entities::EntityId id,
					 const Karkinolution::Math::Vec::Vec2        &position,
					 const Karkinolution::Math::Units::Size      &size,
					 const Karkinolution::Terrain::Territory     &territory);

			bool remove(Karkinolution::Organisms::Entities::EntityId id,
						const Karkinolution::Math::Units::Size      &old_size,
						const Karkinolution::Math::Vec::Vec2        &old_position);
			bool remove(Karkinolution::Organisms::Entities::EntityId id);

			std::vector<Karkinolution::Organisms::Entities::EntityId>
			find(const Karkinolution::Math::Units::Size &size,
				 const Karkinolution::Math::Vec::Vec2   &position) const;

			bool has_entity(Karkinolution::Organisms::Entities::EntityId id) const;

			std::optional<Karkinolution::Organisms::Entities::EntityId>
				 if_find_one_stop(const Karkinolution::Math::Units::Size &size,
								  const Karkinolution::Math::Vec::Vec2   &position) const;
			bool update_coord(Karkinolution::Organisms::Entities::EntityId id,
							  const Karkinolution::Math::Units::Size      &current_size,
							  const Karkinolution::Math::Vec::Vec2        &current_position,
							  const Karkinolution::Math::Vec::Vec2        &new_position,
							  const Karkinolution::Terrain::Territory     &territory);

			bool can_contain(const Karkinolution::Math::Units::Size &size,
							 const Karkinolution::Math::Vec::Vec2   &position) const;
	};

} // namespace Karkinolution::Terrain
