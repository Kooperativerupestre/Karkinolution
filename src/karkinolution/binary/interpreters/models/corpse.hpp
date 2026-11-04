#pragma once

#include "karkinolution/core/id.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/organism/values.hpp"

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace Models {
				namespace Corpse {
					struct Corpse {
							Karkinolution::Core::Id::BaseIdType id;
							Karkinolution::Organisms::RawMeat   raw_meat;
							Karkinolution::Math::Vec::Vec3      position;
							Karkinolution::Math::Units::Size    size;
					};
				} // namespace Corpse
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
