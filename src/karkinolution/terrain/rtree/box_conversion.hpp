#pragma once
#include "karkinolution/organism/nature/grass/grass.hpp"

#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/nature/natures.hpp>
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/soil.hpp>
using GeometryForms::Radius;

namespace BoxConversion {
	Box3D to_box(const Size &size, const Vec3 &position);
	Box3D to_box(const Radius &radius, const Vec3 &position);
	Box3D to_box(const SoilPiece &soil);
	Box3D to_box(const Grass &grass);
	Box3D to_box(const Nature &nature);
} // namespace BoxConversion

template <typename T>
concept BoxConvertible = requires(const T &value) {
	{ BoxConversion::to_box(value) } -> std::same_as<Box3D>;
};