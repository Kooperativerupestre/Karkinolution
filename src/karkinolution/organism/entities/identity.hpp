#pragma once
#include <cstdint>
#include <unordered_map>

enum class EntityTypes : uint8_t {
	CREATURE,
	CORPSE,
	EMBRYO,
	EGG
};

struct Id {
		uint64_t    value;
		EntityTypes type;

		bool operator==(const Id &) const = default;
};

template <> struct std::hash<Id> {
		std::size_t operator()(const Id &id) const noexcept {
			std::size_t h1 = std::hash<uint64_t>{}(id.value);
			std::size_t h2 = std::hash<EntityTypes>{}(id.type);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
};

namespace IDF {
inline constexpr Id create_creature_id(uint64_t id) {
	return Id{id, EntityTypes::CREATURE};
}

inline constexpr Id create_corpse_id(uint64_t id) {
	return Id{id, EntityTypes::CORPSE};
}

inline constexpr Id create_embryo_id(uint64_t id) {
	return Id{id, EntityTypes::EMBRYO};
}

inline constexpr Id create_egg_id(uint64_t id) {
	return Id{id, EntityTypes::EGG};
}
} // namespace IDF
