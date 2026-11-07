#pragma once
#include <cstdint>
#include <karkinolution/organism/internal_aliases.hpp>
#include <shared/math/stats/runtime_values.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Reproduction {
				namespace Uterus {
					class Gestation : public IntegerWithMax<uint16_t> {
						public:

							using IntegerWithMax::IntegerWithMax;
							float    progress() const;
							bool     is_ready_to_born() const;
							uint16_t overdue() const;
					};

				} // namespace Uterus

			} // namespace Reproduction
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
