#pragma once
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "shared/core/id.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/world.hpp>
#include <shared/functional/optional_reference.hpp>
#include <vector>

namespace Karkinolution::API::Creature {
	using CreatureReference =
		KShared::Functional::OptionalReference<const Karkinolution::Organisms::Entities::Creature::Creature>;

	CreatureReference see_creature(const Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);
	std::vector<KShared::Core::Id::BaseIdType> see_all_creature_ids(const Karkinolution::World::World &world);
	CreatureReference                          make_creature(Karkinolution::World::World                                                 &world,
															 std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender,
															 std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie,
															 const std::optional<KShared::Math::Vec::Vec2> &position,
															 const std::optional<std::string>              &name);

	// Runs one creature tick and returns the updated creature, or an empty reference if it does not exist anymore.
	CreatureReference tick(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);

	// Erases a creature and returns whether the creature existed and was removed.
	bool erase(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);
} // namespace Karkinolution::API::Creature
