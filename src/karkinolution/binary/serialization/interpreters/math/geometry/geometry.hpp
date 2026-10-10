#pragma once

#include "karkinolution/binary/serialization/interpreters/math/unit/unit.hpp"

#include <karkinolution/binary/serialization/interpreters/math/unit/alias.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/math/geometry/models.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Geometry {
					namespace Layout {
						namespace Sizes {
							inline constexpr std::size_t RADIUS        = 8;
							inline constexpr std::size_t CIRCUMFERENCE = 8;
							inline constexpr std::size_t DIAMETER      = 8;
							inline constexpr std::size_t AREA          = 8;
						} // namespace Sizes

					} // namespace Layout

					UnitsSRI::StandardPhysicBytes serialize_radius(GeometryForms::Radius radius);
					UnitsSRI::StandardPhysicBytes
					serialize_circumference(GeometryForms::Circumference circumference);
					UnitsSRI::StandardPhysicBytes
					serialize_diameter(GeometryForms::Diameter diameter);
					UnitsSRI::StandardPhysicBytes serialize_area(GeometryForms::Area area);
				} // namespace Geometry
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
