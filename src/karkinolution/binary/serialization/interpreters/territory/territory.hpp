#pragma once

#include <array>
#include <cstddef>
#include <karkinolution/binary/serialization/interpreters/math/unit/unit.hpp>
#include <karkinolution/terrain/terrain.hpp>

namespace TerritorySRI {

	/*
	 * Territory protocol
	 *
	 * # begin size (24 bytes)
	 * [0 -> 7 - size.lateral]
	 * [8 -> 15 - size.height]
	 * [16 -> 23 - size.depth]
	 * # end size
	 */

	inline constexpr std::size_t SIZE_BYTES = sizeof(double) * 3;

	inline constexpr std::size_t TERRITORY_BYTES = SIZE_BYTES;

	inline constexpr std::size_t TO_GET_SIZE_OFFSET = 0;

	static_assert(TERRITORY_BYTES == TO_GET_SIZE_OFFSET + SIZE_BYTES);

	using TerritoryBytes = std::array<std::byte, TERRITORY_BYTES>;

	TerritoryBytes serialize_territory(const Territory &territory);

} // namespace TerritorySRI
