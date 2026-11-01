#include <karkinolution/math/global_epsilon.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>

namespace Karkinolution::Organisms::Entities::Corpse {

bool Corpse::ready_to_disapear() const {
	return raw_meat.value == Math::Approx<float>(0.0f);
}

} // namespace Karkinolution::Organisms::Entities::Corpse
