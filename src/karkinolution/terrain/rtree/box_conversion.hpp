#pragma once
#include "karkinolution/math/units.hpp"
#include "karkinolution/organism/nature/grass/grass.hpp"

#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/nature/natures.hpp>
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/soil/soil.hpp>

namespace Karkinolution {
	namespace Terrain {
		namespace Trees {
			namespace RTree::BoxConversion {
				Karkinolution::Terrain::Trees::RTree::Box2D to_box(const Math::Units::Size &size,
																   const Vec2 &position);
				Karkinolution::Terrain::Trees::RTree::Box2D
				to_box(const Math::Geometry::Radius &radius, const Vec2 &position);
				Karkinolution::Terrain::Trees::RTree::Box2D
				to_box(const Karkinolution::Terrain::Soil::SoilPiece &soil);
				Karkinolution::Terrain::Trees::RTree::Box2D
				to_box(const Karkinolution::Organisms::Nature::Grass::Grass &grass);
				Karkinolution::Terrain::Trees::RTree::Box2D
				to_box(const Karkinolution::Organisms::Nature::NatureEntity &nature_entity);
			} // namespace RTree::BoxConversion

			template <typename T>
			concept BoxConvertible = requires(const T &value) {
				{
					RTree::BoxConversion::to_box(value)
				} -> std::same_as<Karkinolution::Terrain::Trees::RTree::Box2D>;
			};

		} // namespace Trees
	} // namespace Terrain
} // namespace Karkinolution
