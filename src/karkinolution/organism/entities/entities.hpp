#pragma once
#include "karkinolution/math/physic/vec/model.hpp"

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/pathogens/identity.hpp>
#include <karkinolution/organism/pathogens/pathogen.hpp>
#include <karkinolution/organism/pathogens/virus/virus.hpp>
#include <karkinolution/organism/reproduction/oviparous/egg.hpp>

using Entity                   = std::variant<Creature, Corpse, Embryo, Egg>;
using GenericPathogenContainer = std::variant<VirusContainer>;

class EntitiesRegistry : public BaseStorage<EntityId, Entity> {
	public:

		EntitiesRegistry()
			: BaseStorage<EntityId, Entity>() {}

		Creature       &at_creature(EntityId id);
		const Creature &at_creature(EntityId id) const;

		Corpse       &at_corpse(EntityId id);
		const Corpse &at_corpse(EntityId id) const;

		Embryo       &at_embryo(EntityId id);
		const Embryo &at_embryo(EntityId id) const;

		Egg       &at_egg(EntityId id);
		const Egg &at_egg(EntityId id) const;
};

namespace EntityGetters {
	EntityId get_id(const Entity &entity);
	EntityId get_id(const Corpse &corpse);
	EntityId get_id(const Creature &creature);
	EntityId get_id(const Embryo &embryo);
	EntityId get_id(const Egg &egg);

	Vec3       &get_position(Entity &entity);
	const Vec3 &get_position(const Entity &entity);

	Size get_size(const Entity &entity);

} // namespace EntityGetters
