#pragma once

#include <cstdint>
#include <karkinolution/core/id.hpp>

enum class NatureTypes : uint8_t {
	GRASS
};

struct NatureId {
		NatureTypes type;
		BaseIdType  value;

		bool operator==(const NatureId &) const = default;
};

template <> struct std::hash<NatureId> {
		std::size_t operator()(const NatureId &id) const noexcept {
			std::size_t h1 = std::hash<BaseIdType>{}(id.value);
			std::size_t h2 = std::hash<NatureTypes>{}(id.type);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
};

namespace NatureIDF {
	inline constexpr NatureId create_grass_id(BaseIdType id) {
		return NatureId{.type = NatureTypes::GRASS, .value = id};
	}
} // namespace NatureIDF