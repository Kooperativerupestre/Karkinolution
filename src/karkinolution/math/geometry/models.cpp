#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/constants.hpp>

[[nodiscard]] constexpr GeometryForms::Circumference GeometryForms::Radius::circumference() const noexcept {
    return Circumference{value * Constants::two_pi};
}

[[nodiscard]] constexpr GeometryForms::Diameter GeometryForms::Radius::diameter() const noexcept {
    return Diameter{2.0 * value};
}

[[nodiscard]] constexpr GeometryForms::Area GeometryForms::Radius::area() const noexcept {
    return Area{Constants::pi * value * value};
}