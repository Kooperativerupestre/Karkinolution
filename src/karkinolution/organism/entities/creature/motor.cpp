#include "karkinolution/organism/reproduction/state/motor.hpp"

#include "karkinolution/organism/entities/creature/actions/presets.hpp"
#include "karkinolution/organism/entities/creature/metabolism/resolver.hpp"
#include "karkinolution/organism/entities/creature/movement/motor.hpp"

#include <karkinolution/organism/entities/creature/brain/instincts/instincts.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perceiver.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/metabolism/motor.hpp>
#include <karkinolution/organism/entities/creature/motor.hpp>
#include <karkinolution/organism/entities/creature/physiology.hpp>
#include <karkinolution/world/world.hpp>
#include <variant>

namespace Karkinolution::Organisms::Entities::Creature::Brain::Motor {
	void BrainMotor::run(Karkinolution::Organisms::Entities::Creature::Brain::Brain &brain,
						 const Creature                                             &creature) {
		const auto stop = Physiology::BrainPhysiology::should_stop_intent(brain);

		if (stop) {
			brain.switch_intent(
				Karkinolution::Organisms::Entities::Creature::Brain::Instincts::Instincts::take(
					creature));
		}

		brain.pass_intent();
	}
} // namespace Karkinolution::Organisms::Entities::Creature::Brain::Motor

namespace Karkinolution::Organisms::Entities::Creature::Motor {
	void CreatureMotor::update_map(const Creature &creature, Karkinolution::World::World &world) {
		world.update(creature.build_id(), creature.position);
	}

	void
	CreatureMotor::grow(Creature                                                   &creature,
						const Karkinolution::Organisms::Entities::EntitiesRegistry &organisms) {
		auto &body        = creature.body;
		auto  width_grow  = Physiology::Growing::get_new_width_increment(creature, organisms);
		auto  height_grow = Physiology::Growing::get_new_height_increment(creature, organisms);

		body.morphology.size.height.value += height_grow;
		body.morphology.size.width.value += width_grow;

		auto max_energy_grow =
			Physiology::Growing::get_new_max_energy_increment(creature, organisms);
		body.metabolism.energy.max_ref() += max_energy_grow.gain;
		body.metabolism.reserved -= max_energy_grow.cost.reserved_energy;

		auto max_reserved_grow =
			Physiology::Growing::get_new_max_energy_reserved_increment(creature, organisms);
		body.metabolism.reserved.max_ref() += max_reserved_grow.gain;
		body.metabolism.reserved -= max_reserved_grow.cost.reserved_energy;
	}

	std::optional<Actions::AllPresets> CreatureMotor::resolve_presets(
		Creature &creature,
		const Karkinolution::Organisms::Entities::Creature::Brain::Perception::Perception
									&perception,
		Karkinolution::World::World &world) {
		const auto preset_ =
			Karkinolution::Organisms::Entities::Creature::Brain::Instincts::Planner::plan(
				creature,
				perception);

		if (!preset_.has_value()) {
			return std::nullopt;
		}
		const auto &preset = preset_.value();

		if (std::holds_alternative<Actions::MovePreset>(preset)) {
			Karkinolution::Organisms::Entities::Creature::Movement::Motor::MovementMotor::move(
				creature,
				world,
				std::get<Actions::MovePreset>(preset).new_coord);
		} else if (std::holds_alternative<Actions::EatPreset>(preset)) {
			Karkinolution::Organisms::Entities::Creature::Metabolism::Motor::MetabolismMotor::eat(
				creature,
				std::get<std::reference_wrapper<RawMeat>>(
					Karkinolution::Organisms::Entities::Creature::Metabolism::Resolver::
						MetabolismResolver::resolve_preset(std::get<Actions::EatPreset>(preset),
														   world))
					.get());
		}
		return preset_;
	}

	void CreatureMotor::run(Creature &creature, Karkinolution::World::World &world) {
		// aliases

		const auto &organisms = world.see_entities_registry();

		// code
		if (Physiology::is_dead(creature)) {
			world.remove(creature.build_id());
		}

		grow(creature, organisms);

		if (creature.body.reproduction.state.is_pregnant()) {
			Reproduction::State::Motor::run(creature);
		}

		const Karkinolution::Organisms::Entities::Creature::Brain::Perception::Perception
			perception = Karkinolution::Organisms::Entities::Creature::Brain::Perception::
				Perceiver::perceive(creature, world);
		Karkinolution::Organisms::Entities::Creature::Brain::Motor::BrainMotor::run(creature.brain,
																					creature);

		const auto preset = resolve_presets(creature, perception, world);
	}
} // namespace Karkinolution::Organisms::Entities::Creature::Motor
