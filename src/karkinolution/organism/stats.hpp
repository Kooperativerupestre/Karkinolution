#pragma once
#include <cstdint>
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/math/units.hpp>

using PhysicsStats::Efficiency;
using PhysicsStats::Quality;
using PhysicsStats::Volume;

namespace OrganismStats {
	class Energy : public RuntimeLimitedValue<Energy, float> {
		public:

			using RuntimeLimitedValue<Energy, float>::RuntimeLimitedValue;
	};

	namespace Body {
		class Strength : public FloatValue {
				using FloatValue::FloatValue;
		};

		class Damage : public FloatValue {
				using FloatValue::FloatValue;
		};
	} // namespace Body

	class Life : public RuntimeLimitedValue<Life, float> {
		public:

			using RuntimeLimitedValue<Life, float>::RuntimeLimitedValue;
	};

	namespace Time {
		class FertilityCooldown : public IntegerWithMax<uint8_t> {
			public:

				using IntegerWithMax<uint8_t>::IntegerWithMax;

				bool reproductive_capabable() const {
					return value() > max();
				}
		};

		class Age : public FloatValue {
			public:

				using FloatValue::FloatValue;
		};

		class GrowthRate : public FloatValue {
			public:

				using FloatValue::FloatValue;
		};
	} // namespace Time

	namespace Muscle {
		class Muscle : public FloatValue {
			public:

				using FloatValue::FloatValue;
		};

		class MuscleEfficiency : public Efficiency {
				using Efficiency::Efficiency;
		};

		class MuscleQuality : public Quality {
				using Quality::Quality;
		};
	} // namespace Muscle

	namespace Skeleton {
		class Bone : public FloatValue {
				using FloatValue::FloatValue;
		};

		class SkeletonQuality : public Quality {
				using Quality::Quality;
		};

		class Toughness : public NormalizedValue<float> {
				using NormalizedValue::NormalizedValue;
		};
	} // namespace Skeleton

	namespace Health {
		class Immunity : public NormalizedValue<float> {
				using NormalizedValue::NormalizedValue;
		};

		class Health : public NormalizedValue<float> {
				using NormalizedValue::NormalizedValue;
		};
	} // namespace Health

} // namespace OrganismStats
