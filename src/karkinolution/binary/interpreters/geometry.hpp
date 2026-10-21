#pragma once

#include "karkinolution/binary/interpreters/unit.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/math/geometry/models.hpp"

#include <cstddef>
#include <vector>

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

			namespace DSI {
				namespace Geometry {
					GeometryForms::Radius deserialize_radius(const std::vector<std::byte> &payload,
															 std::size_t                   offset);
					GeometryForms::Circumference
					deserialize_circumference(const std::vector<std::byte> &payload,
											  std::size_t                   offset);
					GeometryForms::Diameter
					deserialize_diameter(const std::vector<std::byte> &payload, std::size_t offset);
					GeometryForms::Area deserialize_area(const std::vector<std::byte> &payload,
														 std::size_t                   offset);
				} // namespace Geometry
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace GeometrySRI = Karkinolution::Binary::Interpreters::SRI::Geometry;
namespace GeometryDSI = Karkinolution::Binary::Interpreters::DSI::Geometry;
