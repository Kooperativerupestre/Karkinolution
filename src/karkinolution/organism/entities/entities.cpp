#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities//embryo/embryo.hpp>

// ============================================================================
// EntitiesRegistry
// ============================================================================

Creature& EntitiesRegistry::at_creature(Id id) {
    return std::get<Creature>(this->at(id));
}

const Creature& EntitiesRegistry::at_creature(Id id) const {
    return std::get<Creature>(this->at(id));
}

Corpse& EntitiesRegistry::at_corpse(Id id) {
    return std::get<Corpse>(this->at(id));
}

const Corpse& EntitiesRegistry::at_corpse(Id id) const {
    return std::get<Corpse>(this->at(id));
}

Embryo& EntitiesRegistry::at_embryo(Id id) {
    return std::get<Embryo>(this->at(id));
}

const Embryo& EntitiesRegistry::at_embryo(Id id) const {
    return std::get<Embryo>(this->at(id));
}

// ============================================================================
// EntityGetters
// ============================================================================

Id EntityGetters::get_id(const Entity& entity) {
    return std::visit([](const auto& value) {
        return get_id(value);
    }, entity);
}

Id EntityGetters::get_id(const Corpse& corpse) {
    return IDF::create_corpse_id(corpse.id);
}

Id EntityGetters::get_id(const Creature& creature) {
    return IDF::create_creature_id(creature.ontology.id);
}

Id EntityGetters::get_id(const Embryo& embryo) {
    return IDF::create_embryo_id(embryo.id);
}

