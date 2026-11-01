#include "karkinolution/organism/nature/natures.hpp"

#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <utility>

namespace Karkinolution::Terrain::Trees::RTree::BoxConversion {

	Box3D to_box(const Soil::SoilPiece &soil) {
		Vec3 offset{soil.radius.value, soil.radius.value, 0.0};
		return Karkinolution::Terrain::Trees::RTree::Box3D{.max = soil.position + offset,
														   .min = soil.position - offset};
	}

	Box3D to_box(const Organisms::Nature::Grass::Grass &grass) {
		Vec3 offset{grass.radius.value, grass.radius.value, 0.0};
		return Karkinolution::Terrain::Trees::RTree::Box3D{.max = grass.position + offset,
														   .min = grass.position - offset};
	}

	Box3D to_box(const Organisms::Nature::NatureEntity &nature_entity) {
		if (std::holds_alternative<Organisms::Nature::Grass::Grass>(nature_entity)) {
			return Karkinolution::Terrain::Trees::RTree::BoxConversion::to_box(
				std::get<Organisms::Nature::Grass::Grass>(nature_entity));
		}
		std::unreachable();
	}

	Box3D to_box(const Math::Units::Size &size, const Vec3 &position) {
		Vec3 half_extents{size.lateral.value / 2, size.height.value / 2, size.depth.value / 2};
		return Karkinolution::Terrain::Trees::RTree::Box3D{.max = position + half_extents,
														   .min = position - half_extents};
	}

	Box3D to_box(const Math::Geometry::Radius &radius, const Vec3 &center) {
		Vec3 extent{radius.value, radius.value, radius.value};
		return Karkinolution::Terrain::Trees::RTree::Box3D{.max = center + extent,
														   .min = center - extent};
	}

} // namespace Karkinolution::Terrain::Trees::RTree::BoxConversion
