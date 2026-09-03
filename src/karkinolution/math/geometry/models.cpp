#include <karkinolution/math/constants.hpp>
#include <karkinolution/math/geometry/models.hpp>

[[nodiscard]] GeometryForms::Circumference GeometryForms::Radius::circumference() const noexcept {
	return Circumference{value * Constants::two_pi};
}

[[nodiscard]] GeometryForms::Diameter GeometryForms::Radius::diameter() const noexcept {
	return Diameter{2.0 * value};
}

[[nodiscard]] GeometryForms::Area GeometryForms::Radius::area() const noexcept {
	return Area{Constants::pi * value * value};
}