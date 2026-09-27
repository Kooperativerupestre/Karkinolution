#pragma once
#include <karkinolution/math/units.hpp>

namespace GeometryForms {
	class Circumference : public Meter {
		public:

			using Meter::Meter;
	};

	class Diameter : public Meter {
		public:

			using Meter::Meter;
	};

	class Area : public Meter {
		public:

			using Meter::Meter;
	};

	class Radius : public Meter {
		public:

			using Meter::Meter;

			[[nodiscard]] Circumference circumference() const noexcept;
			[[nodiscard]] Diameter      diameter() const noexcept;
			[[nodiscard]] Area          area() const noexcept;
	};
} // namespace GeometryForms
