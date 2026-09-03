#include <karkinolution/core/global_epsilon.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>

bool Corpse::ready_to_disapear() const {
	return raw_meat.energy == Approx<float>(0.0f);
}
