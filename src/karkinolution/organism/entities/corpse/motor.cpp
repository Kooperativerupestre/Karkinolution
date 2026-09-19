#include "karkinolution/world/motor.hpp"

#include "karkinolution/world/world.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/corpse/motor.hpp>
#include <karkinolution/organism/registry.hpp>

void CorpseMotor::run(Corpse &corpse, World &world) {
	corpse.raw_meat.value *= 0.1;

	if (corpse.ready_to_disapear()) {
		WorldMotor::remove(corpse.build_id(), world);
	}
}
