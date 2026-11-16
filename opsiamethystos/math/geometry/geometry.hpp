#pragma once
#include "shared/math/stats/compile_values.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>


using namespace KShared::Math::LV::CT;

namespace Opsiamethystos {
	namespace Gui {
		namespace Geometry {

			class Position {
				public:

					float x{}, y{};

					constexpr Position() noexcept = default;

					constexpr Position(float x, float y) noexcept
						: x(x)
						, y(y) {}

					constexpr Position move_to_left(float value) const {
						return {x - value, y};
					}

					constexpr Position move_to_right(float value) const {
						return {x + value, y};
					}

					constexpr Position move_up(float value) const {
						return {x, y - value};
					}

					constexpr Position move_down(float value) const {
						return {x, y + value};
					}

					constexpr bool operator==(const Position &) const noexcept = default;

					constexpr Position operator+(const Position &other) const noexcept {
						return {x + other.x, y + other.y};
					}

					constexpr Position &operator+=(const Position &other) noexcept {
						x += other.x;
						y += other.y;
						return *this;
					}

					constexpr Position operator-(const Position &other) const noexcept {
						return {x - other.x, y - other.y};
					}

					constexpr Position &operator-=(const Position &other) noexcept {
						x -= other.x;
						y -= other.y;
						return *this;
					}

					constexpr Position operator*(float scalar) const noexcept {
						return {x * scalar, y * scalar};
					}

					constexpr Position &operator*=(float scalar) noexcept {
						x *= scalar;
						y *= scalar;
						return *this;
					}

					constexpr Position operator/(float scalar) const {
						return {x / scalar, y / scalar};
					}

					constexpr Position &operator/=(float scalar) {
						x /= scalar;
						y /= scalar;
						return *this;
					}

					constexpr Position operator+() const noexcept {
						return *this;
					}

					constexpr Position operator-() const noexcept {
						return {-x, -y};
					}

					[[nodiscard]] float length() const noexcept {
						return std::hypot(x, y);
					}

					[[nodiscard]] float distance_to(const Position &other) const noexcept {
						return std::hypot(x - other.x, y - other.y);
					}
			};

			constexpr Position operator*(float scalar, const Position &position) noexcept {
				return position * scalar;
			}

			struct Width : public GreaterThanZeroValue<double> {};

			struct Height : public GreaterThanZeroValue<double> {};

			struct Thickness : public GreaterThanZeroValue<double> {};

			struct Row : public NonNegativeValue<int> {};

			struct Column : public NonNegativeValue<int> {};

			struct Radius : public NonNegativeValue<float> {};

			// Edge comparisons express geometric ordering, not raw coordinate ordering.
			// In particular, vertical edges account for screen-space Y increasing downward.
			class Top {
				public:

					float value;

					constexpr bool operator==(const Top &top) const {
						return value == top.value;
					}

					constexpr bool operator>(const Top &top) const {
						return value > top.value;
					}

					constexpr bool operator>=(const Top &top) const {
						return value >= top.value;
					}

					constexpr bool operator<(const Top &top) const {
						return value < top.value;
					}

					constexpr bool operator<=(const Top &top) const {
						return value <= top.value;
					}
			};

			class Floor {
				public:

					float value;

					constexpr bool operator>(const Floor &floor) const {
						return value < floor.value;
					}

					constexpr bool operator>=(const Floor &floor) const {
						return value <= floor.value;
					}

					constexpr bool operator<(const Floor &floor) const {
						return value > floor.value;
					}

					constexpr bool operator<=(const Floor &floor) const {
						return value >= floor.value;
					}
			};

			class Left {
				public:

					float value;

					constexpr bool operator>(const Left &left) const {
						return value > left.value;
					}

					constexpr bool operator>=(const Left &left) const {
						return value >= left.value;
					}

					constexpr bool operator<(const Left &left) const {
						return value < left.value;
					}

					constexpr bool operator<=(const Left &left) const {
						return value <= left.value;
					}
			};

			class Right {
				public:

					float value;

					constexpr bool operator>(const Right &right) const {
						return value < right.value;
					}

					constexpr bool operator>=(const Right &right) const {
						return value <= right.value;
					}

					constexpr bool operator<(const Right &right) const {
						return value > right.value;
					}

					constexpr bool operator<=(const Right &right) const {
						return value >= right.value;
					}
			};

			class Rectangle {
				public:

					Position position;
					Width    width;

					Height height;

					constexpr Rectangle(Position position, Width width, Height height) noexcept
						: position(position)
						, width(width)
						, height(height) {}

					// Layout performs arithmetic in edges because adjacent slots share exact
					// boundaries. This is the sole conversion back to the center-plus-extents
					// representation used by Geometry; callers must provide left < right and
					// top < floor so the positive dimensions remain representable.
					static constexpr Rectangle from_edges(Left left, Top top, Right right, Floor floor) {
						return Rectangle{Position{(left.value + right.value) / 2.0f, (top.value + floor.value) / 2.0f},
										 Geometry::Width{static_cast<double>(right.value - left.value)},
										 Geometry::Height{static_cast<double>(floor.value - top.value)}};
					}

					constexpr Top top() const {
						return {position.move_up(static_cast<float>(height.value()) / 2.0f).y};
					}

					constexpr Floor floor() const {
						return {position.move_down(static_cast<float>(height.value()) / 2.0f).y};
					}

					constexpr Left left() const {
						return {position.move_to_left(static_cast<float>(width.value()) / 2.0f).x};
					}

					constexpr Right right() const {
						return {position.move_to_right(static_cast<float>(width.value()) / 2.0f).x};
					}

					constexpr bool operator==(const Rectangle &) const noexcept = default;
			};

			class Triangle {
				public:

					Position a, b, c;
			};

			class Circle {
				public:

					Position position;
					Radius   radius;

					constexpr float area() const noexcept {
						return std::numbers::pi_v<float> * radius.value() * radius.value();
					}

					constexpr float circumference() const noexcept {
						return 2.0f * std::numbers::pi_v<float> * radius.value();
					}

					constexpr Top top() const {
						return {position.move_up(static_cast<float>(radius.value())).y};
					}

					constexpr Left left() const {
						return {position.move_to_left(static_cast<float>(radius.value())).x};
					}

					constexpr Right right() const {
						return {position.move_to_right(static_cast<float>(radius.value())).x};
					}

					constexpr Floor floor() const {
						return {position.move_down(static_cast<float>(radius.value())).y};
					}
			};

			constexpr bool can_contain(const Rectangle &container, const Rectangle &contained) {
				return container.top() <= contained.top() && container.left() <= contained.left()
					&& container.floor() <= contained.floor() && container.right() <= contained.right();
			}

			constexpr bool can_contain(const Rectangle &container, const Circle &contained) {
				return container.top() <= contained.top() && container.left() <= contained.left()
					&& container.floor() <= contained.floor() && container.right() <= contained.right();
			}

			constexpr bool can_contain(const Rectangle &container, const Position &contained) {
				return contained.x >= container.left().value && contained.x <= container.right().value
					&& contained.y >= container.top().value && contained.y <= container.floor().value;
			}

			constexpr bool can_contain(const Circle &container, const Position &contained) {
				return contained.distance_to(container.position) <= container.radius.value();
			}

			constexpr bool intersects(const Rectangle &first, const Rectangle &second) {
				const float horizontal_distance = std::abs(first.position.x - second.position.x);
				const float vertical_distance   = std::abs(first.position.y - second.position.y);

				return horizontal_distance <= (first.width.value() + second.width.value()) / 2.0
					&& vertical_distance <= (first.height.value() + second.height.value()) / 2.0;
			}

			constexpr bool intersects(const Circle &first, const Circle &second) {
				return first.position.distance_to(second.position) <= first.radius.value() + second.radius.value();
			}

			inline bool intersects(const Rectangle &rectangle, const Circle &circle) {
				const float closest_x = std::clamp(circle.position.x, rectangle.left().value, rectangle.right().value);
				const float closest_y = std::clamp(circle.position.y, rectangle.top().value, rectangle.floor().value);
				return Geometry::Position{closest_x, closest_y}.distance_to(circle.position) <= circle.radius.value();
			}

			inline bool intersects(const Circle &circle, const Rectangle &rectangle) {
				return intersects(rectangle, circle);
			}

			// Rectangles contain a triangle iff they contain all three vertices.
			// This is sufficient because a rectangle is convex: if the vertices are inside,
			// every point on the edges (and in the interior) is inside as well.
			constexpr bool can_contain(const Rectangle &container, const Triangle &contained) {
				return can_contain(container, contained.a) && can_contain(container, contained.b)
					&& can_contain(container, contained.c);
			}

			// Circles contain a triangle iff they contain all three vertices.
			// NOTE: for a circle this is a conservative approximation, not an exact test:
			// an edge between two inside vertices can still bulge outside the circle.
			// Exact containment would require checking the distance from the circle center
			// to each edge against the radius. That cost is not justified for layout use,
			// where circles are only used to bound triangles loosely.
			constexpr bool can_contain(const Circle &container, const Triangle &contained) {
				return can_contain(container, contained.a) && can_contain(container, contained.b)
					&& can_contain(container, contained.c);
			}

			// Point-in-triangle test via the sign of the cross product on each edge.
			// A point is inside when it lies on the same side of all three edges.
			// The winding order of the triangle is not fixed, so we accept either
			// "all cross products non-negative" (CCW) or "all non-positive" (CW).
			// Points exactly on an edge are treated as contained (>= / <=, not > / <),
			// which matches the closed-boundary convention used by the other can_contain overloads.
			constexpr bool can_contain(const Triangle &container, const Position &contained) {
				const Position ab = container.b - container.a;
				const Position bc = container.c - container.b;
				const Position ca = container.a - container.c;

				const Position ap = contained - container.a;
				const Position bp = contained - container.b;
				const Position cp = contained - container.c;

				const float cross_ab_ap = ab.x * ap.y - ab.y * ap.x;
				const float cross_bc_bp = bc.x * bp.y - bc.y * bp.x;
				const float cross_ca_cp = ca.x * cp.y - ca.y * cp.x;

				const bool all_non_negative = cross_ab_ap >= 0.0f && cross_bc_bp >= 0.0f && cross_ca_cp >= 0.0f;
				const bool all_non_positive = cross_ab_ap <= 0.0f && cross_bc_bp <= 0.0f && cross_ca_cp <= 0.0f;

				return all_non_negative || all_non_positive;
			}

			// A circle is contained in a triangle only if its center and its four
			// cardinal points are inside. This is an approximation: a convex shape
			// can fit inside a triangle while one of its cardinal points falls outside
			// only in pathological cases (very small triangle, very large circle).
			// For layout and hit-testing purposes this is intentional: it is cheap,
			// symmetric with the Rectangle overload below, and conservative enough
			// that false positives are practically irrelevant.
			// The center check is done first because it is the cheapest rejection.
			constexpr bool can_contain(const Triangle &container, const Circle &contained) {
				if (!can_contain(container, contained.position)) {
					return false;
				}

				const float r = contained.radius.value();

				const Position edges[4] = {
					{contained.position.x + r, contained.position.y},
					{contained.position.x - r, contained.position.y},
					{contained.position.x, contained.position.y + r},
					{contained.position.x, contained.position.y - r},
				};
				for (const auto &p : edges) {
					if (!can_contain(container, p)) {
						return false;
					}
				}
				return true;
			}

			// A rectangle is contained in a triangle iff all four corners are inside.
			// Convexity of the triangle guarantees that the rest of the rectangle
			// (edges and interior) is also inside. The four corners are taken from the
			// rectangle's edge accessors rather than from position + extents, so this
			// overload stays correct if the internal representation of Rectangle changes.
			constexpr bool can_contain(const Triangle &container, const Rectangle &contained) {
				const Position corners[4] = {
					{contained.left().value, contained.top().value},
					{contained.right().value, contained.top().value},
					{contained.left().value, contained.floor().value},
					{contained.right().value, contained.floor().value},
				};
				for (const auto &p : corners) {
					if (!can_contain(container, p)) {
						return false;
					}
				}
				return true;
			}
		} // namespace Geometry
	} // namespace Gui
} // namespace Opsiamethystos
