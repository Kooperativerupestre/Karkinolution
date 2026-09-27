#pragma once

#include <cstddef>
#include <karkinolution/math/geometry/models.hpp>
#include <vector>

namespace GeometryDSI {
	GeometryForms::Radius deserialize_radius(const std::vector<std::byte> &payload,
											 std::size_t                   offset = 0);
	GeometryForms::Circumference
	deserialize_circumference(const std::vector<std::byte> &payload, std::size_t offset = 0);
	GeometryForms::Diameter deserialize_diameter(const std::vector<std::byte> &payload,
												 std::size_t                   offset = 0);
	GeometryForms::Area     deserialize_area(const std::vector<std::byte> &payload,
											 std::size_t                   offset = 0);
} // namespace GeometryDSI
