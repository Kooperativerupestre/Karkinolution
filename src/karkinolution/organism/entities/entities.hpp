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

using Entity = std::variant<Creature, Corpse, Embryo, Egg>;
using GenericPathogenContainer = std::variant<VirusContainer>;

class EntitiesRegistry : public BaseStorage<Id, Entity> {
  public:
    EntitiesRegistry() : BaseStorage<Id, Entity>() {}

    Creature& at_creature(Id id);
    const Creature& at_creature(Id id) const;

    Corpse& at_corpse(Id id);
    const Corpse& at_corpse(Id id) const;

    Embryo& at_embryo(Id id);
    const Embryo& at_embryo(Id id) const;

    Egg& at_egg(Id id);
    const Egg& at_egg(Id id) const;
};

namespace EntityGetters {
Id get_id(const Entity& entity);
Id get_id(const Corpse& corpse);
Id get_id(const Creature& creature);
Id get_id(const Embryo& embryo);
Id get_id(const Egg& egg);

Vec3& get_position(Entity& entity);
const Vec3& get_position(const Entity& entity);

Size get_size(Entity& entity);
} // namespace EntityGetters
