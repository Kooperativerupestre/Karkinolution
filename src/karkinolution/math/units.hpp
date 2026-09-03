#pragma once
#include <karkinolution/core/error.hpp>
#include <karkinolution/math/stats/compile_values.hpp>

class FloatValue {
	public:

		float value;

		FloatValue(float value)
			: value(static_cast<float>(value)) {}
};

class DoubleValue {
	public:

		double value;

		DoubleValue(double value)
			: value(static_cast<double>(value)) {}
};

template <typename T> class IntegerValue {
	public:

		T value;

		template <typename U>

		IntegerValue(U value)
			: value(static_cast<T>(value)) {}
};

namespace PhysicsStats {
	class Volume : public DoubleValue {
		public:

			using DoubleValue::DoubleValue;
	};

	class SharedVolume : public NormalizedValue<double> {
		public:

			using NormalizedValue::NormalizedValue;
	};

	class Efficiency : public NormalizedValue<float> {
		public:

			using NormalizedValue::NormalizedValue;
	};

	class Quality : public NormalizedValue<float> {
		public:

			using NormalizedValue::NormalizedValue;
	};

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
} // namespace PhysicsStats

using PhysicsStats::Efficiency;
using PhysicsStats::Meter;
using PhysicsStats::Volume;

struct Lateral : public Meter {
		using Meter::Meter;
};

struct Height : public Meter {
		using Meter::Meter;
};

struct Back : public Meter {
		using Meter::Meter;
};

class Size {
	public:

		Lateral lateral;
		Height  height;
		Back    back;

		static Volume volume(Lateral lateral, Height height, Back back) {
			return Volume{lateral.value * height.value * back.value};
		}

		void modify(const NormalizedValue<float> &ratio) {
			lateral.value *= ratio.value();
			height.value *= ratio.value();
			back.value *= ratio.value();
		}

		Volume volume() const {
			return Size::volume(lateral, height, back);
		}
};
