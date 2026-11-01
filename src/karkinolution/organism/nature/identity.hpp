#pragma once

#include <cstdint>
#include <karkinolution/core/id.hpp>
#include <karkinolution/organism/internal_aliases.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Nature {
			enum class NatureTypes : uint8_t {
				GRASS
			};

			struct NatureId {
					NatureTypes type;
					BaseIdType  value;

					bool operator==(const NatureId &) const = default;
			};

			inline constexpr NatureId create_grass_id(BaseIdType id) {
				return NatureId{.type = NatureTypes::GRASS, .value = id};
			}

		} // namespace Nature
	} // namespace Organisms
} // namespace Karkinolution

template <> struct std::hash<Karkinolution::Organisms::Nature::NatureId> {
		std::size_t
		operator()(const Karkinolution::Organisms::Nature::NatureId &id) const noexcept {
			std::size_t h1 = std::hash<Karkinolution::Core::Id::BaseIdType>{}(id.value);
			std::size_t h2 = std::hash<Karkinolution::Organisms::Nature::NatureTypes>{}(id.type);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
};