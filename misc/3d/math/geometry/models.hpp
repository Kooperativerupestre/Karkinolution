#pragma once
#include <karkinolution/math/units.hpp>

namespace Karkinolution {
	namespace Math {
		namespace Geometry {
			class Circumference : public Units::Meter {
				public:

					using Meter::Meter;
			};

			class Diameter : public Units::Meter {
				public:

					using Meter::Meter;
			};

			class Area : public Units::Meter {
				public:

					using Meter::Meter;
			};

			class Radius : public Units::Meter {
				public:

					using Meter::Meter;

					[[nodiscard]] Circumference circumference() const noexcept;
					[[nodiscard]] Diameter      diameter() const noexcept;
					[[nodiscard]] Area          area() const noexcept;
			};
		} // namespace Geometry
	} // namespace Math
} // namespace Karkinolution
