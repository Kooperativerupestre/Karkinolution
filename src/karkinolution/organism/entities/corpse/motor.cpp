#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/corpse/motor.hpp>
#include <karkinolution/organism/registry.hpp>

void CorpseMotor::run(Corpse &corpse) {
	corpse.raw_meat.energy *= 0.1;
}