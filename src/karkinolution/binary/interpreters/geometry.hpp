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

					UnitsSRI::StandardPhysicBytes serialize_radius(Karkinolution::Math::Geometry::Radius radius);
					UnitsSRI::StandardPhysicBytes
					serialize_circumference(Karkinolution::Math::Geometry::Circumference circumference);
					UnitsSRI::StandardPhysicBytes serialize_diameter(Karkinolution::Math::Geometry::Diameter diameter);
					UnitsSRI::StandardPhysicBytes serialize_area(Karkinolution::Math::Geometry::Area area);
				} // namespace Geometry
			} // namespace SRI

			namespace DSI {
				namespace Geometry {
					Karkinolution::Math::Geometry::Radius deserialize_radius(const std::vector<std::byte> &payload,
																			 std::size_t                   offset);
					Karkinolution::Math::Geometry::Circumference
					deserialize_circumference(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Math::Geometry::Diameter deserialize_diameter(const std::vector<std::byte> &payload,
																				 std::size_t                   offset);
					Karkinolution::Math::Geometry::Area     deserialize_area(const std::vector<std::byte> &payload,
																			 std::size_t                   offset);
				} // namespace Geometry
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace GeometrySRI = Karkinolution::Binary::Interpreters::SRI::Geometry;
namespace GeometryDSI = Karkinolution::Binary::Interpreters::DSI::Geometry;
