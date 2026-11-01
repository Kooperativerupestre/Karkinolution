#pragma once
#include <karkinolution/core/error.hpp>
#include <karkinolution/math/stats/compile_values.hpp>

namespace Karkinolution {

	namespace Math {
		class FloatValue {
			public:

				float value;

				FloatValue(float value)
					: value(static_cast<float>(value)) {}
		};

		class DoubleValue {
			public:

				double value;

				constexpr DoubleValue(double value)
					: value(static_cast<double>(value)) {}
		};

		template <typename T> class IntegerValue {
			public:

				T value;

				template <typename U>

				IntegerValue(U value)
					: value(static_cast<T>(value)) {}
		};

		class Volume : public DoubleValue {
			public:

				using DoubleValue::DoubleValue;
		};

		class SharedVolume : public LV::CT::NormalizedValue<double> {
			public:

				using NormalizedValue::NormalizedValue;
		};

		class Efficiency : public LV::CT::NormalizedValue<float> {
			public:

				using NormalizedValue::NormalizedValue;
		};

		class Quality : public LV::CT::NormalizedValue<float> {
			public:

				using NormalizedValue::NormalizedValue;
		};

		namespace Units {
			class Mass : public DoubleValue {
				public:

					using DoubleValue::DoubleValue;
			};

			class Density : public DoubleValue {
				public:

					using DoubleValue::DoubleValue;
			};

			class Meter : public DoubleValue {
				public:

					using DoubleValue::DoubleValue;
			};

			struct Lateral : public Meter {
					using Meter::Meter;
			};

			struct Height : public Meter {
					using Meter::Meter;
			};

			struct Depth : public Meter {
					using Meter::Meter;
			};

			class Size {
				public:

					Lateral lateral;
					Height  height;
					Depth   depth;

					static Volume volume(Lateral lateral, Height height, Depth back) {
						return Volume{lateral.value * height.value * back.value};
					}

					void modify(const LV::CT::NormalizedValue<float> &ratio) {
						lateral.value *= ratio.value();
						height.value *= ratio.value();
						depth.value *= ratio.value();
					}

					Volume volume() const {
						return Size::volume(lateral, height, depth);
					}
			};
		} // namespace Units


	} // namespace Math
} // namespace Karkinolution