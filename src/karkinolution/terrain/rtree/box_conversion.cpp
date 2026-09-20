#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <utility>

Box3D BoxConversion::to_box(const SoilPiece &soil) {
	Vec3 offset{soil.radius.value, soil.radius.value, 0.0};
	return Box3D{.max = soil.position + offset, .min = soil.position - offset};
}

Box3D BoxConversion::to_box(const Grass &grass) {
	Vec3 offset{grass.radius.value, grass.radius.value, 0.0};
	return Box3D{.max = grass.position + offset, .min = grass.position - offset};
}

Box3D BoxConversion::to_box(const Nature &nature) {
	if (std::holds_alternative<Grass>(nature)) {
		return BoxConversion::to_box(std::get<Grass>(nature));
	}
	std::unreachable();
}

Box3D BoxConversion::to_box(const Size &size, const Vec3 &position) {
	Vec3 half_extents{size.lateral.value / 2, size.height.value / 2, size.back.value / 2};
	return Box3D{.max = position + half_extents, .min = position - half_extents};
}

Box3D BoxConversion::to_box(const Radius &radius, const Vec3 &center) {
	Vec3 extent{radius.value, radius.value, radius.value};
	return Box3D{.max = center + extent, .min = center - extent};
}
