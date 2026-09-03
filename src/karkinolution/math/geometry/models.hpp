#pragma once
#include <karkinolution/math/units.hpp>

namespace GeometryForms {
	class Circumference : public Meter {
			using Meter::Meter;
	};

	class Diameter : public Meter {
			using Meter::Meter;
	};

	class Area : public Meter {
			using Meter::Meter;
	};

	class Radius : public Meter {
			using Meter::Meter;

			[[nodiscard]] Circumference circumference() const noexcept;
			[[nodiscard]] Diameter      diameter() const noexcept;
			[[nodiscard]] Area          area() const noexcept;
	};
} // namespace GeometryForms