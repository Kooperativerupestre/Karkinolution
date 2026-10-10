#pragma once
#include "karkinolution/binary/fields.hpp"

#include <array>
#include <cstddef>
#include <karkinolution/math/physic/vec/model.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Vec {
					namespace Layout {
						inline constexpr std::size_t AXIS_SIZE = sizeof(double);

						using X = Field<0, AXIS_SIZE>;
						using Y = Next<X, AXIS_SIZE>;
						using Z = Next<Y, AXIS_SIZE>;

						inline constexpr std::size_t TOTAL_SIZE = Z::END;
					} // namespace Layout

					namespace BytesArrays {
						using Vec = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					/*
					 * Always, a Vec3 must be like this
					 * [x](8 bytes)[y](8 bytes)[z](8 bytes)
					 */

					BytesArrays::Vec serialize_vec(const Vec3 &vec);
				} // namespace Vec
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
