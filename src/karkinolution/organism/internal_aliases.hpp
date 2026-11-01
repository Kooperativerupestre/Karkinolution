#pragma once

/*

Don't include this file outside src/karkinolution/organism
*/

#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/math/stats/runtime_values.hpp"
#include "karkinolution/math/units.hpp"

namespace Karkinolution {
	namespace Organisms {
		using Karkinolution::Math::LV::CT::GenericLimitedValue;
		using Karkinolution::Math::LV::CT::LimitedValue;
		using Karkinolution::Math::LV::CT::NormalizedValue;
		using Karkinolution::Math::LV::CT::SignedNormalizedValue;

		using Karkinolution::Core::Id::BaseIdType;
		using Karkinolution::Errors::SimulationError;
		using Karkinolution::Math::DoubleValue;
		using Karkinolution::Math::Efficiency;
		using Karkinolution::Math::FloatValue;
		using Karkinolution::Math::Quality;
		using Karkinolution::Math::SharedVolume;
		using Karkinolution::Math::Volume;
		using Karkinolution::Math::LV::RT::IntegerLimited;
		using Karkinolution::Math::LV::RT::IntegerWithMax;
		using Karkinolution::Math::LV::RT::RuntimeLimitedValue;
		using Karkinolution::Math::Units::Density;
		using Karkinolution::Math::Units::Mass;
		using Karkinolution::Math::Units::Size;
		namespace KRandom = Karkinolution::Functional::KRandom;
		using Karkinolution::Math::LV::CT::GreaterThanZeroValue;
		using Karkinolution::Math::Units::Depth;
		using Karkinolution::Math::Units::Height;
		using Karkinolution::Math::Units::Lateral;
	} // namespace Organisms
} // namespace Karkinolution
