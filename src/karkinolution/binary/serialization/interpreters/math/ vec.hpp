#pragma once
#include <array>
#include <cstddef>
#include <karkinolution/math/physic/vec/model.hpp>

namespace VecSRI {
	inline constexpr std::size_t AXIS_BYTES = sizeof(double);

	inline constexpr std::size_t TO_GET_X_OFFSET = 0;
	inline constexpr std::size_t TO_GET_Y_OFFSET = AXIS_BYTES;
	inline constexpr std::size_t TO_GET_Z_OFFSET = AXIS_BYTES * 2;

	using VecBytes = std::array<std::byte, AXIS_BYTES * 3>;

	/*
	 * Always, a Vec3 must be like this
	 * [x](8 bytes)[y](8 bytes)[z](8 bytes)
	 */

	VecBytes serialize_vec(const Vec3 &vec);
} // namespace VecSRI