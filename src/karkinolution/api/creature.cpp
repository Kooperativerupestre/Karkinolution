#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/identity.hpp"

#include <karkinolution/api/creature.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/motor.hpp>
#include <karkinolution/world/world.hpp>

const Creature* CreatureAPI::see_creature(const World &world, BaseIdType id) {
	const auto creature =
		world.organism_registry.entities.try_at(EntityIDF::create_creature_id(id));

	if (creature == nullptr) {
		return nullptr;
	}
	return &std::get<Creature>(*creature);
}

const Creature* CreatureAPI::make_creature(World                            &world,
										   std::optional<Gender>             gender,
										   std::optional<CreatureSpecies>    specie,
										   const std::optional<Vec3>        &position,
										   const std::optional<std::string> &name) {
	auto creature = CreatureConstructor::from_blueprint(
		CreatureBlueprint{.specie = specie, .name = name, .gender = gender, .position = position});

	const auto creature_id = creature.build_id();

	if (WorldMotor::add(std::move(creature), world)) {
		return nullptr;
	}

	auto &creature_ref = world.organism_registry.entities.at_creature(creature_id);

	return &creature_ref;
}