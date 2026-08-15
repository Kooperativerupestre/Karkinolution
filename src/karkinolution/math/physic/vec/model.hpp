#pragma once
#include <cmath>
#include <iostream>

class Vec3 {
    public:
    double x, y, z;

    constexpr Vec3(double x, double y, double z) noexcept : x(x), y(y), z(z) {}

    constexpr Vec3 operator+(const Vec3& other) const noexcept {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    constexpr Vec3 operator-(const Vec3& other) const noexcept {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    constexpr Vec3 operator*(double scalar) const noexcept {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    constexpr Vec3 operator/(double scalar) const noexcept {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    constexpr Vec3 operator-() const noexcept {
        return Vec3(-x, -y, -z);
    }

    constexpr bool operator==(const Vec3& other) const noexcept {
        return x == other.x && y == other.y && z == other.z;
    }

    constexpr bool operator!=(const Vec3& other) const noexcept {
        return !(*this == other);
    }

    [[nodiscard]] constexpr double length() const noexcept {
        return std::sqrt(x * x + y * y + z * z);
    }

    [[nodiscard]] constexpr double length_squared() const noexcept {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] constexpr Vec3 normalized() const noexcept {
        const double len = length();
        return len > 0.0 ? Vec3(x / len, y / len, z / len) : Vec3(0.0, 0.0, 0.0);
    }

    constexpr void normalize() noexcept {
        const double len = length();
        if (len > 0.0) {
            const double inv_len = 1.0 / len;
            x *= inv_len; y *= inv_len; z *= inv_len;
        }
    }

    [[nodiscard]] constexpr double dot(const Vec3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] constexpr Vec3 cross(const Vec3& other) const noexcept {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    [[nodiscard]] constexpr double distance_to(const Vec3& other) const noexcept {
        return (*this - other).length();
    }

    [[nodiscard]] constexpr double distance_squared_to(const Vec3& other) const noexcept {
        return (*this - other).length_squared();
    }

    [[nodiscard]] constexpr Vec3 clamped(double max_length) const noexcept {
        const double len_sq = length_squared();
        if (len_sq <= max_length * max_length) return *this;
        const double scale = max_length / std::sqrt(len_sq);
        return Vec3(x * scale, y * scale, z * scale);
    }

    [[nodiscard]] constexpr Vec3 lerp(const Vec3& other, double t) const noexcept {
        return Vec3(
            x + (other.x - x) * t,
            y + (other.y - y) * t,
            z + (other.z - z) * t
        );
    }

    [[nodiscard]] constexpr Vec3 reflect(const Vec3& normal) const noexcept {
        return *this - normal * (2.0 * dot(normal));
    }

    [[nodiscard]] constexpr bool approx_equal(const Vec3& other, double epsilon = 1e-10) const noexcept {
        return std::abs(x - other.x) <= epsilon &&
               std::abs(y - other.y) <= epsilon &&
               std::abs(z - other.z) <= epsilon;
    }

    [[nodiscard]] constexpr bool is_zero(double epsilon = 1e-10) const noexcept {
        return length_squared() <= epsilon * epsilon;
    }

    [[nodiscard]] constexpr bool is_unit(double epsilon = 1e-10) const noexcept {
        return std::abs(length_squared() - 1.0) <= epsilon;
    }

    [[nodiscard]] constexpr bool is_parallel(const Vec3& other, double epsilon = 1e-10) const noexcept {
        return cross(other).is_zero(epsilon);
    }

    [[nodiscard]] constexpr bool is_perpendicular(const Vec3& other, double epsilon = 1e-10) const noexcept {
        return std::abs(dot(other)) <= epsilon;
    }

    [[nodiscard]] constexpr Vec3 abs() const noexcept {
        return Vec3(std::abs(x), std::abs(y), std::abs(z));
    }

    [[nodiscard]] constexpr double max_component() const noexcept {
        return std::max({x, y, z});
    }

    [[nodiscard]] constexpr double min_component() const noexcept {
        return std::min({x, y, z});
    }

    [[nodiscard]] constexpr size_t max_component_index() const noexcept {
        if (x >= y && x >= z) return 0;
        if (y >= z) return 1;
        return 2;
    }

    [[nodiscard]] constexpr size_t min_component_index() const noexcept {
        if (x <= y && x <= z) return 0;
        if (y <= z) return 1;
        return 2;
    }
};

constexpr Vec3 operator*(double scalar, const Vec3& vec) noexcept {
    return vec * scalar;
}

inline std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
    os << "Vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

inline std::istream& operator>>(std::istream& is, Vec3& vec) {
    is >> vec.x >> vec.y >> vec.z;
    return is;
}
