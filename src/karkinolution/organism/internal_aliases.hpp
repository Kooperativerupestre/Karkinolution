#pragma once

/*

Don't include this file outside src/karkinolution/organism
*/

#include "shared/core/error.hpp"
#include "shared/core/id.hpp"
#include "shared/math/stats/compile_values.hpp"
#include "shared/math/stats/runtime_values.hpp"
#include "shared/math/units.hpp"

namespace Karkinolution {
	namespace Organisms {
		using KShared::Math::LV::CT::GenericLimitedValue;
		using KShared::Math::LV::CT::LimitedValue;
		using KShared::Math::LV::CT::NormalizedValue;
		using KShared::Math::LV::CT::SignedNormalizedValue;

		using KShared::Core::Id::BaseIdType;
		using KShared::Errors::SimulationError;
		using KShared::Math::Area;
		using KShared::Math::DoubleValue;
		using KShared::Math::Efficiency;
		using KShared::Math::FloatValue;
		using KShared::Math::Quality;
		using KShared::Math::SharedArea;
		using KShared::Math::LV::RT::IntegerLimited;
		using KShared::Math::LV::RT::IntegerWithMax;
		using KShared::Math::LV::RT::RuntimeLimitedValue;
		using KShared::Math::Units::Density;
		using KShared::Math::Units::Mass;
		using KShared::Math::Units::Size;
		namespace KRandom = KShared::Functional::KRandom;
		using KShared::Math::LV::CT::GreaterThanZeroValue;
		using KShared::Math::Units::Height;
		using KShared::Math::Units::Width;
	} // namespace Organisms
} // namespace Karkinolution
