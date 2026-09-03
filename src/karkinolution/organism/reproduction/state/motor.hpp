#pragma once
#include "karkinolution/organism/entities/creature/creature.hpp"
#include "karkinolution/organism/entities/embryo/embryo.hpp"

#include <cstdint>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

enum class ConceiveOutput : uint8_t {
	ALREADY_PREGNANT,
	OK
};

namespace ReproductionStateMotor {
void                                 run(Creature &creature);
void                                 prepair_to_conceive(Creature &creature);
std::variant<Embryo, ConceiveOutput> conceive(Creature &, Creature &);
} // namespace ReproductionStateMotor