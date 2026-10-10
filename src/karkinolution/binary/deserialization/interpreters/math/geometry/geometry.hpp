#pragma once

#include <cstddef>
#include <karkinolution/math/geometry/models.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
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
