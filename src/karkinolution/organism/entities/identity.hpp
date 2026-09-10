#pragma once
#include "karkinolution/core/id.hpp"

#include <cstdint>
#include <functional>

enum class EntityTypes : uint8_t {
	CREATURE,
	CORPSE,
	EMBRYO,
	EGG
};

struct EntityId {
		BaseIdType  value;
		EntityTypes type;

		bool operator==(const EntityId &) const = default;
};

template <> struct std::hash<EntityId> {
		std::size_t operator()(const EntityId &id) const noexcept {
			std::size_t h1 = std::hash<BaseIdType>{}(id.value);
			std::size_t h2 = std::hash<EntityTypes>{}(id.type);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
};

namespace EntityIDF {
	inline constexpr EntityId create_creature_id(BaseIdType id) {
		return EntityId{id, EntityTypes::CREATURE};
	}

	inline constexpr EntityId create_corpse_id(BaseIdType id) {
		return EntityId{id, EntityTypes::CORPSE};
	}

	inline constexpr EntityId create_embryo_id(BaseIdType id) {
		return EntityId{id, EntityTypes::EMBRYO};
	}

	inline constexpr EntityId create_egg_id(BaseIdType id) {
		return EntityId{id, EntityTypes::EGG};
	}
} // namespace EntityIDF
