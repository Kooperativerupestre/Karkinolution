#pragma once

#include "karkinolution/core/id.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/organism/stats.hpp"

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace Models {
				namespace Corpse {
					struct Corpse {
							BaseIdType                   id;
							OrganismStats::Body::RawMeat raw_meat;
							Vec3                         position;
							Size                         size;
					};
				} // namespace Corpse
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
