#pragma once
#include <karkinolution/organism/entities/egg/egg.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <shared/math/stats/compile_values.hpp>

namespace Karkinolution::Organisms::Entities::Egg::Physiology {
	Karkinolution::Organisms::Entities::Embryo::EmbryonicEnergy
	embryotrophy(const Egg                                                &egg,
				 const Karkinolution::Organisms::Entities::Embryo::Embryo &embryo,
				 const NormalizedValue<float>                             &ratio);
} // namespace Karkinolution::Organisms::Entities::Egg::Physiology
