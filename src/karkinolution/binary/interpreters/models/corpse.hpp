#pragma once

#include "shared/core/id.hpp"
#include "shared/math/physic/vec/model.hpp"
#include "shared/math/units.hpp"
#include "karkinolution/organism/values.hpp"

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace Models {
				namespace Corpse {
					struct Corpse {
							KShared::Core::Id::BaseIdType id;
							Karkinolution::Organisms::RawMeat   raw_meat;
							KShared::Math::Vec::Vec2      position;
							KShared::Math::Units::Size    size;
					};
				} // namespace Corpse
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
