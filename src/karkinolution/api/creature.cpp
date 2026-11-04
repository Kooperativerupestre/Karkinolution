#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/identity.hpp"

#include <karkinolution/api/creature.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::API::Creature {

	const Karkinolution::Organisms::Entities::Creature::Creature*
	see_creature(const Karkinolution::World::World &world, Karkinolution::Core::Id::BaseIdType id) {
		const auto entity_id = Karkinolution::Organisms::Entities::create_creature_id(id);

		if (!world.see_entities_registry().exists(entity_id)) {
			return nullptr;
		}
		return &world.at_creature(entity_id);
	}

	const Karkinolution::Organisms::Entities::Creature::Creature* make_creature(
		Karkinolution::World::World                                                 &world,
		std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender,
		std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie,
		const std::optional<Karkinolution::Math::Vec::Vec3>                         &position,
		const std::optional<std::string>                                            &name) {
		auto creature = Karkinolution::Organisms::Entities::Creature::Constructor::
			CreatureConstructor::from_blueprint(
				Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint{
					.specie   = specie,
					.name     = name,
					.gender   = gender,
					.position = position});

		const auto creature_id = creature.build_id();

		if (world.add(std::move(creature))) {
			return nullptr;
		}

		auto &creature_ref = world.at_creature(creature_id);

		return &creature_ref;
	}

} // namespace Karkinolution::API::Creature