#include <shared/math/constants.hpp>
#include <shared/math/geometry/models.hpp>

using namespace KShared::Math;

[[nodiscard]] Geometry::Circumference Geometry::Radius::circumference() const noexcept {
	return Circumference{value * Constants::two_pi};
}

[[nodiscard]] Geometry::Diameter Geometry::Radius::diameter() const noexcept {
	return Diameter{2.0 * value};
}

[[nodiscard]] Geometry::Area Geometry::Radius::area() const noexcept {
	return Area{Constants::pi * value * value};
}
