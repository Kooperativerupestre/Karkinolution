#include <karkinolution/organism/entities/corpse/corpse.hpp>


bool Corpse::ready_to_disapear() const {
    return energy.is_zero();
}

