#include "shared/math/physic/vec/model.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <utility>

// ============================================================================
// EntitiesRegistry
// ============================================================================
namespace Karkinolution::Organisms::Entities {

	bool EntitiesRegistry::add(Entity &&entity) {
		return KShared::Functional::try_add(registry, EntityGetters::get_id(entity), std::move(entity));
	}

	Entity &EntitiesRegistry::at(EntityId id) {
		return registry.at(id);
	}

	const Entity &EntitiesRegistry::at(EntityId id) const {
		return registry.at(id);
	}

	Karkinolution::Organisms::Entities::Creature::Creature &
	EntitiesRegistry::at_creature(Karkinolution::Organisms::Entities::EntityId id) {
		return std::get<Creature::Creature>(this->at(id));
	}

	const Karkinolution::Organisms::Entities::Creature::Creature &
	EntitiesRegistry::at_creature(Karkinolution::Organisms::Entities::EntityId id) const {
		return std::get<Creature::Creature>(this->at(id));
	}

	Corpse::Corpse &EntitiesRegistry::at_corpse(Karkinolution::Organisms::Entities::EntityId id) {
		return std::get<Corpse::Corpse>(this->at(id));
	}

	const Corpse::Corpse &EntitiesRegistry::at_corpse(Karkinolution::Organisms::Entities::EntityId id) const {
		return std::get<Corpse::Corpse>(this->at(id));
	}

	Embryo::Embryo &EntitiesRegistry::at_embryo(Karkinolution::Organisms::Entities::EntityId id) {
		return std::get<Embryo::Embryo>(this->at(id));
	}

	const Embryo::Embryo &EntitiesRegistry::at_embryo(Karkinolution::Organisms::Entities::EntityId id) const {
		return std::get<Embryo::Embryo>(this->at(id));
	}

	Egg::Egg &EntitiesRegistry::at_egg(Karkinolution::Organisms::Entities::EntityId id) {
		return std::get<Egg::Egg>(this->at(id));
	}

	const Egg::Egg &EntitiesRegistry::at_egg(Karkinolution::Organisms::Entities::EntityId id) const {
		return std::get<Egg::Egg>(this->at(id));
	}

	// ============================================================================
	// EntityGetters
	// ============================================================================

	Karkinolution::Organisms::Entities::EntityId EntityGetters::get_id(const Entity &entity) {
		return std::visit(
			[](const auto &value) {
				return get_id(value);
			},
			entity);
	}

	Karkinolution::Organisms::Entities::EntityId EntityGetters::get_id(const Corpse::Corpse &corpse) {
		return Karkinolution::Organisms::Entities::create_corpse_id(corpse.id);
	}

	Karkinolution::Organisms::Entities::EntityId EntityGetters::get_id(const Creature::Creature &creature) {
		return Karkinolution::Organisms::Entities::create_creature_id(creature.ontology.id);
	}

	Karkinolution::Organisms::Entities::EntityId EntityGetters::get_id(const Embryo::Embryo &embryo) {
		return Karkinolution::Organisms::Entities::create_embryo_id(embryo.id);
	}

	Karkinolution::Organisms::Entities::EntityId EntityGetters::get_id(const Egg::Egg &egg) {
		return egg.build_id();
	}

	KShared::Math::Vec::Vec2 &EntityGetters::get_position(Entity &entity) {
		return std::visit(
			[](auto &entity) -> KShared::Math::Vec::Vec2 & {
				return entity.position;
			},
			entity);
	}

	const KShared::Math::Vec::Vec2 &EntityGetters::get_position(const Entity &entity) {
		return std::visit(
			[](const auto &entity) -> const KShared::Math::Vec::Vec2 & {
				return entity.position;
			},
			entity);
	}

	KShared::Math::Units::Size EntityGetters::get_size(const Entity &entity) {
		if (std::holds_alternative<Creature::Creature>(entity)) {
			return std::get<Creature::Creature>(entity).body.morphology.size;
		} else if (std::holds_alternative<Corpse::Corpse>(entity)) {
			return std::get<Corpse::Corpse>(entity).size;
		} else if (std::holds_alternative<Embryo::Embryo>(entity)) {
			return std::get<Embryo::Embryo>(entity).size();
		} else if (std::holds_alternative<Egg::Egg>(entity)) {
			return std::get<Egg::Egg>(entity).size;
		}
		std::unreachable();
	}

} // namespace Karkinolution::Organisms::Entities
