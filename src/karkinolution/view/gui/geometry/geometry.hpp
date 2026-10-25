#pragma once
#include "karkinolution/math/stats/compile_values.hpp"

#include <cmath>
#include <functional>

using namespace Karkinolution::Math::LV::CompileTime;

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

			class Rectangle {
				public:

					Position position;
					Width    width;

					Height height;

					constexpr Rectangle(Position position, Width width, Height height) noexcept
						: position(position)
						, width(width)
						, height(height) {}

					constexpr bool operator==(const Rectangle &) const noexcept = default;

					// Rectangle + Rectangle
					constexpr Rectangle operator+(const Rectangle &other) const noexcept {
						return {position + other.position,
								Width{width + other.width},
								Height{height + other.height}};
					}

					constexpr Rectangle &operator+=(const Rectangle &other) noexcept {
						position += other.position;
						width += other.width;
						height += other.height;
						return *this;
					}

					// Rectangle - Rectangle
					constexpr Rectangle operator-(const Rectangle &other) const noexcept {
						return {position - other.position,
								Width{width - other.width},
								Height{height - other.height}};
					}

					constexpr Rectangle &operator-=(const Rectangle &other) noexcept {
						position -= other.position;
						width -= other.width;
						height -= other.height;
						return *this;
					}

					// Rectangle * scalar
					constexpr Rectangle operator*(float scalar) const noexcept {
						return {position * scalar, Width{width * scalar}, Height{height * scalar}};
					}

					constexpr Rectangle &operator*=(float scalar) noexcept {
						position *= scalar;
						width *= scalar;
						height *= scalar;
						return *this;
					}

					// Rectangle / scalar
					constexpr Rectangle operator/(float scalar) const {
						return {position / scalar, Width{width / scalar}, Height{height / scalar}};
					}

					constexpr Rectangle &operator/=(float scalar) {
						position /= scalar;
						width /= scalar;
						height /= scalar;
						return *this;
					}

					// Unary +
					constexpr Rectangle operator+() const noexcept {
						return *this;
					}

					// Unary -
					constexpr Rectangle operator-() const noexcept {
						return {-position, Width{-width}, Height{-height}};
					}
			};

			constexpr Rectangle operator*(float scalar, const Rectangle &rectangle) noexcept {
				return rectangle * scalar;
			}

		} // namespace Geometry
	} // namespace Gui
} // namespace KarkinolutionView