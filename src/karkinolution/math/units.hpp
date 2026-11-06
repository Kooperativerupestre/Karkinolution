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

		class Area : public DoubleValue {
			public:

				using DoubleValue::DoubleValue;
		};

		class SharedArea : public LV::CT::NormalizedValue<double> {
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

			struct Width : public Meter {
					using Meter::Meter;
			};

			struct Height : public Meter {
					using Meter::Meter;
			};

			class Size {
				public:

					Width  width;
					Height height;

					static Area area(Width width, Height height) {
						return Area{width.value * height.value};
					}

					void modify(const LV::CT::NormalizedValue<float> &ratio) {
						width.value *= ratio.value();
						height.value *= ratio.value();
					}

					Area area() const {
						return Size::area(width, height);
					}
			};
		} // namespace Units
	} // namespace Math
} // namespace Karkinolution
