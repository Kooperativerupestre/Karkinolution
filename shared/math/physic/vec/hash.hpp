#pragma once
#include <functional>
#include <shared/math/physic/vec/model.hpp>

namespace std {
	template <> struct hash<KShared::Math::Vec::Vec2> {
			size_t operator()(const KShared::Math::Vec::Vec2 &v) const noexcept {
				const size_t h1 = hash<double>()(v.x);
				const size_t h2 = hash<double>()(v.y);
				return h1 ^ (h2 << 1);
			}
	};
} // namespace std
