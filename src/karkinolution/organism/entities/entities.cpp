#include "karkinolution/math/physic/vec/model.hpp"
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <utility>

// ============================================================================
// EntitiesRegistry
// ============================================================================

Creature& EntitiesRegistry::at_creature(Id id) { return std::get<Creature>(this->at(id)); }

const Creature& EntitiesRegistry::at_creature(Id id) const {
    return std::get<Creature>(this->at(id));
}

Corpse& EntitiesRegistry::at_corpse(Id id) { return std::get<Corpse>(this->at(id)); }

const Corpse& EntitiesRegistry::at_corpse(Id id) const { return std::get<Corpse>(this->at(id)); }

Embryo& EntitiesRegistry::at_embryo(Id id) { return std::get<Embryo>(this->at(id)); }

const Embryo& EntitiesRegistry::at_embryo(Id id) const { return std::get<Embryo>(this->at(id)); }

// ============================================================================
// EntityGetters
// ============================================================================

Id EntityGetters::get_id(const Entity& entity) {
    return std::visit([](const auto& value) { return get_id(value); }, entity);
}

Id EntityGetters::get_id(const Corpse& corpse) { return IDF::create_corpse_id(corpse.id); }

Id EntityGetters::get_id(const Creature& creature) {
    return IDF::create_creature_id(creature.ontology.id);
}

Id EntityGetters::get_id(const Embryo& embryo) { return IDF::create_embryo_id(embryo.id); }

Vec3& EntityGetters::get_position(Entity& entity) {
    return std::visit([](auto& entity) -> Vec3& { return entity.position; }, entity);
}

const Vec3& EntityGetters::get_position(const Entity& entity) {
    return std::visit([](const auto& entity) -> const Vec3& { return entity.position; }, entity);
}

Size& EntityGetters::get_size(Entity& entity) {
    if (std::holds_alternative<Creature>(entity)) {
        return std::get<Creature>(entity).body.morphology.size;
    } else if (std::holds_alternative<Corpse>(entity)) {
        return std::get<Corpse>(entity).size;
    } else if (std::holds_alternative<Embryo>(entity)) {
        throw SimulationError("Embryo doesn't have size");
    } else if (std::holds_alternative<Egg>(entity)) {
        return std::get<Egg>(entity).size;
    }
    std::unreachable();
}

const Size& EntityGetters::get_size(const Entity& entity) {
    if (std::holds_alternative<Creature>(entity)) {
        return std::get<Creature>(entity).body.morphology.size;
    } else if (std::holds_alternative<Corpse>(entity)) {
        return std::get<Corpse>(entity).size;
    } else if (std::holds_alternative<Embryo>(entity)) {
        throw SimulationError("Embryo doesn't have size");
    } else if (std::holds_alternative<Egg>(entity)) {
        return std::get<Egg>(entity).size;
    }
    std::unreachable();
}