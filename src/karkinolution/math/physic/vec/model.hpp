#pragma once
#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace Karkinolution {
	namespace Math {
		namespace Vec {
			enum class Axis : uint8_t {
				X,
				Y
			};

			template <typename T>
			concept Vec2Like = requires(T value) {
				requires std::same_as<std::remove_cvref_t<decltype(value.x)>, double>;
				requires std::same_as<std::remove_cvref_t<decltype(value.y)>, double>;
			};

			class Vec2 {
				public:

					double x, y;

					constexpr Vec2(double x, double y) noexcept
						: x(x)
						, y(y) {}

					constexpr Vec2()
						: x(0.0)
						, y(0.0) {}

					constexpr Vec2 operator+(const Vec2 &other) const noexcept {
						return Vec2(x + other.x, y + other.y);
					}

					constexpr Vec2* operator+=(const Vec2 &other) noexcept {
						x += other.x;
						y += other.y;
						return this;
					}

					constexpr Vec2 operator-(const Vec2 &other) const noexcept {
						return Vec2(x - other.x, y - other.y);
					}

					constexpr Vec2* operator-=(const Vec2 &other) noexcept {
						x -= other.x;
						y -= other.y;
						return this;
					}

					constexpr Vec2 operator*(double scalar) const noexcept {
						return Vec2(x * scalar, y * scalar);
					}

					constexpr Vec2* operator*=(double scalar) noexcept {
						x *= scalar;
						y *= scalar;
						return this;
					}

					constexpr Vec2 operator/(double scalar) const {
						return Vec2(x / scalar, y / scalar);
					}

					constexpr Vec2* operator/=(double scalar) {
						x /= scalar;
						y /= scalar;
						return this;
					}

					constexpr Vec2 operator-() const noexcept {
						return Vec2(-x, -y);
					}

					constexpr bool operator==(const Vec2 &other) const noexcept {
						return x == other.x && y == other.y;
					}

					constexpr bool operator!=(const Vec2 &other) const noexcept {
						return !(*this == other);
					}

					[[nodiscard]] constexpr double length() const noexcept {
						return std::sqrt(x * x + y * y);
					}

					[[nodiscard]] constexpr double length_squared() const noexcept {
						return x * x + y * y;
					}

					[[nodiscard]] constexpr Vec2 normalized() const noexcept {
						const double len = length();
						return len > 0.0 ? Vec2(x / len, y / len) : Vec2(0.0, 0.0);
					}

					constexpr void normalize() noexcept {
						const double len = length();
						if (len > 0.0) {
							const double inv_len = 1.0 / len;
							x *= inv_len;
							y *= inv_len;
						}
					}

					[[nodiscard]] constexpr double dot(const Vec2 &other) const noexcept {
						return x * other.x + y * other.y;
					}

					[[nodiscard]] constexpr double distance_to(const Vec2 &other) const noexcept {
						return (*this - other).length();
					}

					[[nodiscard]] constexpr double
					distance_squared_to(const Vec2 &other) const noexcept {
						return (*this - other).length_squared();
					}

					[[nodiscard]] constexpr Vec2 clamped(double max_length) const noexcept {
						const double len_sq = length_squared();
						if (len_sq <= max_length * max_length) {
							return *this;
						}
						const double scale = max_length / std::sqrt(len_sq);
						return Vec2(x * scale, y * scale);
					}

					[[nodiscard]] constexpr Vec2 lerp(const Vec2 &other, double t) const noexcept {
						return Vec2(x + (other.x - x) * t, y + (other.y - y) * t);
					}

					[[nodiscard]] constexpr Vec2 reflect(const Vec2 &normal) const noexcept {
						return *this - normal * (2.0 * dot(normal));
					}

					[[nodiscard]] constexpr bool
					approx_equal(const Vec2 &other, double epsilon = 1e-10) const noexcept {
						return std::abs(x - other.x) <= epsilon && std::abs(y - other.y) <= epsilon;
					}

					[[nodiscard]] constexpr bool is_zero(double epsilon = 1e-10) const noexcept {
						return length_squared() <= epsilon * epsilon;
					}

					[[nodiscard]] constexpr bool is_unit(double epsilon = 1e-10) const noexcept {
						return std::abs(length_squared() - 1.0) <= epsilon;
					}

					[[nodiscard]] constexpr bool
					is_perpendicular(const Vec2 &other, double epsilon = 1e-10) const noexcept {
						return std::abs(dot(other)) <= epsilon;
					}

					[[nodiscard]] constexpr Vec2 abs() const noexcept {
						return Vec2(std::abs(x), std::abs(y));
					}

					[[nodiscard]] constexpr double max_component() const noexcept {
						return std::max({x, y});
					}

					[[nodiscard]] constexpr double min_component() const noexcept {
						return std::min({x, y});
					}

					[[nodiscard]] constexpr size_t max_component_index() const noexcept {
						return x >= y ? 0 : 1;
					}

					[[nodiscard]] constexpr size_t min_component_index() const noexcept {
						return x <= y ? 0 : 1;
					}
			};

			constexpr Vec2 operator*(double scalar, const Vec2 &vec) noexcept {
				return vec * scalar;
			}
		} // namespace Vec
	} // namespace Math
} // namespace Karkinolution
