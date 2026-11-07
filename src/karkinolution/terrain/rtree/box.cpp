#include <algorithm>
#include <shared/math/physic/vec/model.hpp>
#include <karkinolution/terrain/rtree/box.hpp>

namespace Karkinolution::Terrain::Trees::RTree {
	const KShared::Math::Vec::Vec2 &Box2D::operator[](Bound bound) const noexcept {
		return bound == Bound::MAX ? max : min;
	}

	KShared::Math::Vec::Vec2 &Box2D::operator[](Bound bound) noexcept {
		return bound == Bound::MAX ? max : min;
	}

	double Box2D::width() const noexcept {
		return max.x - min.x;
	}

	double Box2D::height() const noexcept {
		return max.y - min.y;
	}

	double Box2D::area() const noexcept {
		return width() * height();
	}

	double Box2D::margin() const noexcept {
		return 2.0 * (width() + height());
	}

	Box2D Box2D::operator+(const Box2D &other) const {
		return Box2D{.max = max + other.max, .min = min + other.min};
	}

	bool Box2DMotor::intersect(const Box2D &a, const Box2D &b) {
		return a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y && a.max.y >= b.min.y;
	}

	Box2D Box2DMotor::intersection(const Box2D &a, const Box2D &b) {
		return Box2D{.max = KShared::Math::Vec::Vec2{std::min(a.max.x, b.max.x), std::min(a.max.y, b.max.y)},
					 .min = KShared::Math::Vec::Vec2{std::max(a.min.x, b.min.x), std::max(a.min.y, b.min.y)}};
	}

	double Box2DMotor::overlap_area(const Box2D &a, const Box2D &b) {
		const double width  = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
		const double height = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
		return width <= 0.0 || height <= 0.0 ? 0.0 : width * height;
	}

	KShared::Math::Vec::Vec2 Box2D::center() const noexcept {
		return {(min.x + max.x) / 2.0, (min.y + max.y) / 2.0};
	}

	Box2D Box2DMotor::combine(const Box2D &a, const Box2D &b) {
		return Box2D{.max = {std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y)},
					 .min = {std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y)}};
	}

	double Box2DMotor::enlargement(const Box2D &current, const Box2D &candidate) {
		return combine(current, candidate).area() - current.area();
	}

	bool Box2DMotor::contains(const Box2D &container, const Box2D &contained) {
		return container.min.x <= contained.min.x && container.min.y <= contained.min.y
			&& container.max.x >= contained.max.x && container.max.y >= contained.max.y;
	}
} // namespace Karkinolution::Terrain::Trees::RTree
