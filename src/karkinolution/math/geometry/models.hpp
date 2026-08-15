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

        [[nodiscard]] constexpr Circumference circumference() const noexcept;
        [[nodiscard]] constexpr Diameter diameter() const noexcept;
        [[nodiscard]] constexpr Area area() const noexcept;
    };
}