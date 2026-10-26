#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/interpreters/models/territory.hpp"
#include "karkinolution/binary/interpreters/unit.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/terrain/terrain.hpp"

#include <array>
#include <cstddef>
#include <vector>

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

			namespace DSI {
				namespace Territory {
					Interpreters::Models::Territory::Territory
					deserialize_territory(const std::vector<std::byte> &payload);
				} // namespace Territory
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace TerritorySRI = Karkinolution::Binary::Interpreters::SRI::Territory;
namespace TerritoryDSI = Karkinolution::Binary::Interpreters::DSI::Territory;
