#pragma once

#include "math/unit/unit.hpp"
#include "math/vec.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>

namespace CorpseSRI {

	/* CORPSE PROTOCOL
	 * [0 -> 7 - id (8 bytes)]
	 * [8 -> 11 - raw_meat (4 bytes)]
	 *
	 * # begin position (24 bytes)
	 * [12 -> 19 - position.x]
	 * [20 -> 27 - position.y]
	 * [28 -> 35 - position.z]
	 * # end position
	 *
	 * # begin size (24 bytes)
	 * [36 -> 43 - size.lateral]
	 * [44 -> 51 - size.height]
	 * [52 -> 59 - size.depth]
	 * # end size
	 */

	inline constexpr std::size_t ID_BYTES       = sizeof(std::uint64_t);
	inline constexpr std::size_t RAW_MEAT_BYTES = sizeof(float);
	inline constexpr std::size_t POSITION_BYTES = VecSRI::AXIS_BYTES * 3;
	inline constexpr std::size_t SIZE_BYTES     = sizeof(double) * 3;

	inline constexpr std::size_t CORPSE_BYTES =
		ID_BYTES + RAW_MEAT_BYTES + POSITION_BYTES + SIZE_BYTES;

	inline constexpr std::size_t TO_GET_ID_OFFSET       = 0;
	inline constexpr std::size_t TO_GET_RAW_MEAT_OFFSET = TO_GET_ID_OFFSET + ID_BYTES;
	inline constexpr std::size_t TO_GET_POSITION_OFFSET = TO_GET_RAW_MEAT_OFFSET + RAW_MEAT_BYTES;
	inline constexpr std::size_t TO_GET_SIZE_OFFSET     = TO_GET_POSITION_OFFSET + POSITION_BYTES;

	static_assert(CORPSE_BYTES == TO_GET_SIZE_OFFSET + SIZE_BYTES);

	using CorpseBytes = std::array<std::byte, CORPSE_BYTES>;

	Serializer::Types::Uint64tBytes serialize_id(std::uint64_t id);
	Serializer::Types::FloatBytes   serialize_raw_meat(const RawMeat &raw_meat);
	VecSRI::VecBytes                serialize_position(const Vec3 &position);
	PhysicsUnitsSRI::SizeBytes      serialize_size(const Size &size);

	CorpseBytes serialize_corpse(const Corpse &corpse);

} // namespace CorpseSRI
