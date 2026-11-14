#pragma once

#include "shared/binary/fields.hpp"
#include "shared/math/physic/vec/model.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace KShared {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Vec {
					namespace Layout {
						inline constexpr std::size_t AXIS_SIZE = sizeof(double);

						using X = KShared::Binary::Field<0, AXIS_SIZE>;
						using Y = KShared::Binary::Next<X, AXIS_SIZE>;

						inline constexpr std::size_t TOTAL_SIZE = Y::END;
					} // namespace Layout

					namespace BytesArrays {
						using Vec = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					/*
					 * Always, a KShared::Math::Vec::Vec2 must be like this
					 * [x](8 bytes)[y](8 bytes)
					 */

					BytesArrays::Vec serialize_vec(const KShared::Math::Vec::Vec2 &vec);
				} // namespace Vec
			} // namespace SRI

			namespace DSI {
				namespace Vec {
					double                   get_x(const std::vector<std::byte> &payload, std::size_t offset);
					double                   get_y(const std::vector<std::byte> &payload, std::size_t offset);
					KShared::Math::Vec::Vec2 get_vec(const std::vector<std::byte> &payload, std::size_t offset);
				} // namespace Vec
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace KShared

namespace VecSRI = KShared::Binary::Interpreters::SRI::Vec;
namespace VecDSI = KShared::Binary::Interpreters::DSI::Vec;
