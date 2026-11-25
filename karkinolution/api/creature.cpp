#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/creature/motor.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "shared/core/id.hpp"

#include <karkinolution/api/creature.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::API::Creature {

	CreatureReference see_creature(const Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id) {
		const auto entity_id = Karkinolution::Organisms::Entities::create_creature_id(id);

		if (!world.see_entities_registry().exists(entity_id)) {
			return nullptr;
		}
		return world.at_creature(entity_id);
	}

	std::vector<KShared::Core::Id::BaseIdType>
	see_all_creature_ids(const Karkinolution::World::World &world) {
		std::vector<KShared::Core::Id::BaseIdType> ids;
		for (const auto &id : world.see_entities_registry().keys()) {
			if (id.type == Karkinolution::Organisms::Entities::EntityTypes::CREATURE) {
				ids.push_back(id.value);
			}
		}
		return ids;
	}

	CreatureReference make_creature(Karkinolution::World::World                                                 &world,
									std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender,
									std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie,
									const std::optional<KShared::Math::Vec::Vec2> &position,
									const std::optional<std::string>              &name) {
		auto creature = Karkinolution::Organisms::Entities::Creature::Constructor::CreatureConstructor::from_blueprint(
			Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint{.specie   = specie,
																						 .name     = name,
																						 .gender   = gender,
																						 .position = position});

		const auto creature_id = creature.build_id();

		if (!world.add(std::move(creature))) {
			return nullptr;
		}

		auto &creature_ref = world.at_creature(creature_id);

		return creature_ref;
	}

	CreatureReference tick(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id) {
		const auto entity_id = Karkinolution::Organisms::Entities::create_creature_id(id);

		if (!world.see_entities_registry().exists(entity_id)) {
			return nullptr;
		}

		Karkinolution::Organisms::Entities::Creature::Motor::CreatureMotor::run(world.at_creature(entity_id), world);
		return see_creature(world, id);
	}

	bool erase(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id) {
		return world.remove(Karkinolution::Organisms::Entities::create_creature_id(id));
	}

} // namespace Karkinolution::API::Creature
