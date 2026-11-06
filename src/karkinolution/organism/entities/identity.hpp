#pragma once
#include "karkinolution/core/id.hpp"

#include <cstdint>
#include <functional>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {

			enum class EntityTypes : uint8_t {
				CREATURE,
				CORPSE,
				EMBRYO,
				EGG
			};

			struct EntityId {
					Core::Id::BaseIdType value;
					EntityTypes          type;

					bool operator==(const EntityId &) const = default;
			};

			inline constexpr EntityId create_creature_id(Core::Id::BaseIdType id) {
				return EntityId{id, EntityTypes::CREATURE};
			}

			inline constexpr EntityId create_corpse_id(Core::Id::BaseIdType id) {
				return EntityId{id, EntityTypes::CORPSE};
			}

			inline constexpr EntityId create_embryo_id(Core::Id::BaseIdType id) {
				return EntityId{id, EntityTypes::EMBRYO};
			}

			inline constexpr EntityId create_egg_id(Core::Id::BaseIdType id) {
				return EntityId{id, EntityTypes::EGG};
			}

		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution

template <> struct ::std::hash<Karkinolution::Organisms::Entities::EntityId> {
		std::size_t operator()(const Karkinolution::Organisms::Entities::EntityId &id) const noexcept {
			std::size_t h1 = std::hash<Karkinolution::Core::Id::BaseIdType>{}(id.value);
			std::size_t h2 = std::hash<Karkinolution::Organisms::Entities::EntityTypes>{}(id.type);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
};
