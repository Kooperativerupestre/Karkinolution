#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/math/physic/vec/model.hpp"

#include <array>
#include <cstddef>
#include <vector>

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

			namespace DSI {
				namespace Vec {
					double get_x(const std::vector<std::byte> &payload, std::size_t offset);
					double get_y(const std::vector<std::byte> &payload, std::size_t offset);
					double get_z(const std::vector<std::byte> &payload, std::size_t offset);
					Vec3   get_vec(const std::vector<std::byte> &payload, std::size_t offset);
				} // namespace Vec
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace VecSRI = Karkinolution::Binary::Interpreters::SRI::Vec;
namespace VecDSI = Karkinolution::Binary::Interpreters::DSI::Vec;
