#pragma once
#include <functional>
#include <karkinolution/math/physic/vec/model.hpp>

namespace std {
	template <> struct hash<Karkinolution::Math::Vec::Vec3> {
			size_t operator()(const Karkinolution::Math::Vec::Vec3 &v) const noexcept {
				size_t h1 = hash<double>()(v.x);
				size_t h2 = hash<double>()(v.y);
				size_t h3 = hash<double>()(v.z);
				return h1 ^ (h2 << 1) ^ (h3 << 2);
			}
	};
} // namespace std
