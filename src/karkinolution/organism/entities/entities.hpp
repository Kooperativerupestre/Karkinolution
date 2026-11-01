#pragma once
#include "karkinolution/functional/base_storage.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/organism/entities/egg/egg.hpp"

#include <karkinolution/functional/base_storage.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/identity.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			using Entity =
				std::variant<Creature::Creature, Corpse::Corpse, Embryo::Embryo, Egg::Egg>;

			class EntitiesRegistry {
				private:

					Functional::BaseStorage<EntityId, Entity> registry;

				public:

					EntitiesRegistry()
						: registry() {}

					bool add(Entity &&entity);

					Creature::Creature       &at_creature(EntityId id);
					const Creature::Creature &at_creature(EntityId id) const;

					Corpse::Corpse       &at_corpse(EntityId id);
					const Corpse::Corpse &at_corpse(EntityId id) const;

					Embryo::Embryo       &at_embryo(EntityId id);
					const Embryo::Embryo &at_embryo(EntityId id) const;

					Egg::Egg       &at_egg(EntityId id);
					const Egg::Egg &at_egg(EntityId id) const;

					Entity       &at(EntityId id);
					const Entity &at(EntityId id) const;

					bool exists(EntityId id) const {
						return registry.exists(id);
					}

					auto keys() const {
						return registry.keys();
					}

					bool remove(EntityId id) {
						return registry.try_del(id);
					}
			};

			namespace EntityGetters {
				EntityId get_id(const Entity &entity);
				EntityId get_id(const Corpse::Corpse &corpse);
				EntityId get_id(const Creature::Creature &creature);
				EntityId get_id(const Embryo::Embryo &embryo);
				EntityId get_id(const Egg::Egg &egg);

				Vec3       &get_position(Entity &entity);
				const Vec3 &get_position(const Entity &entity);

				Size get_size(const Entity &entity);

			} // namespace EntityGetters

		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
