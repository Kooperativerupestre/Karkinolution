#pragma once
#include "model.hpp"

namespace Vec3Utils {
	constexpr Vec3 zero() noexcept {
		return Vec3(0.0, 0.0, 0.0);
	}

	constexpr Vec3 one() noexcept {
		return Vec3(1.0, 1.0, 1.0);
	}

	constexpr Vec3 unit_x() noexcept {
		return Vec3(1.0, 0.0, 0.0);
	}

	constexpr Vec3 unit_y() noexcept {
		return Vec3(0.0, 1.0, 0.0);
	}

	constexpr Vec3 unit_z() noexcept {
		return Vec3(0.0, 0.0, 1.0);
	}

	constexpr Vec3 up() noexcept {
		return unit_y();
	}

	constexpr Vec3 down() noexcept {
		return Vec3(0.0, -1.0, 0.0);
	}

	constexpr Vec3 right() noexcept {
		return unit_x();
	}

	constexpr Vec3 left() noexcept {
		return Vec3(-1.0, 0.0, 0.0);
	}

	constexpr Vec3 forward() noexcept {
		return unit_z();
	}

	constexpr Vec3 backward() noexcept {
		return Vec3(0.0, 0.0, -1.0);
	}
} // namespace Vec3Utils

namespace std {
	template <> struct hash<Vec3> {
			size_t operator()(const Vec3 &v) const noexcept {
				size_t h1 = hash<double>()(v.x);
				size_t h2 = hash<double>()(v.y);
				size_t h3 = hash<double>()(v.z);
				return h1 ^ (h2 << 1) ^ (h3 << 2);
			}
	};
} // namespace std
