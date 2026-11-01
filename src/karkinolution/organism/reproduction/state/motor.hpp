#pragma once
#include "karkinolution/organism/entities/creature/creature.hpp"
#include "karkinolution/organism/entities/embryo/embryo.hpp"

#include <cstdint>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

namespace Karkinolution::Organisms::Entities::Reproduction::State::Motor {

	enum class ConceiveOutput : uint8_t {
		ALREADY_PREGNANT,
		OK
	};

	void run(Karkinolution::Organisms::Entities::Creature::Creature &creature);
	void prepair_to_conceive(Karkinolution::Organisms::Entities::Creature::Creature &creature);
	std::variant<Karkinolution::Organisms::Entities::Embryo::Embryo, ConceiveOutput>
	conceive(Karkinolution::Organisms::Entities::Creature::Creature &,
			 Karkinolution::Organisms::Entities::Creature::Creature &);

} // namespace Karkinolution::Organisms::Entities::Reproduction::State::Motor
