#pragma once

#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/values.hpp>
#include <shared/math/stats/compile_values.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Reproduction {
				namespace Uterus {
					namespace Physiology {
						NormalizedValue<float>  embryotrophy_energy(const Uterus &uterus, const Embryo::Embryo &embryo);
						Embryo::EmbryonicEnergy embryotrophy(const Uterus                 &uterus,
															 const Embryo::Embryo         &embryo,
															 const NormalizedValue<float> &ratio);

					} // namespace Physiology
				} // namespace Uterus
			} // namespace Reproduction
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
