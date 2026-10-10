#pragma once

#include "karkinolution/binary/fields.hpp"

#include <array>
#include <cstddef>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/serialization/interpreters/math/geometry/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/unit/unit.hpp>
#include <karkinolution/terrain/terrain.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Territory {
					/*
					 * Territory protocol
					 *
					 * # begin size (24 bytes)
					 * [0 -> 7 - size.lateral]
					 * [8 -> 15 - size.height]
					 * [16 -> 23 - size.depth]
					 * # end size
					 */

					namespace Layout {
						using Size = Field<0, UnitsSRI::Layout::TOTAL_SIZE>;

						inline size_t constexpr TOTAL_SIZE = Size::END;
					} // namespace Layout

					namespace BytesArrays {
						using Territory = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					BytesArrays::Territory serialize_territory(const class Territory &territory);
				} // namespace Territory
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
