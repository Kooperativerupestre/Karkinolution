#pragma once
#include "shared/math/stats/compile_values.hpp"

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <functional>
#include <spanstream>

using namespace KShared::Math::LV::CT;

namespace KarkinolutionView {
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
		} // namespace Geometry
	} // namespace Gui
} // namespace KarkinolutionView
