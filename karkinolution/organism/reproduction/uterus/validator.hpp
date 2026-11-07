#pragma once
#include <cassert>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Reproduction {
				namespace Uterus {
					namespace Validator {
						inline void is_pregnant(const Uterus &uterus) {
							return assert(uterus.is_pregnant());
						}

						inline void is_empty(const Uterus &uterus) {
							return assert(uterus.is_empty());
						}
					} // namespace Validator
				} // namespace Uterus
			} // namespace Reproduction
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
