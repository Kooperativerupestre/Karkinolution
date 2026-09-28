#pragma once

#include <cstddef>
#include <karkinolution/terrain/terrain.hpp>
#include <vector>

namespace TerritoryDSI {

	Territory deserialize_territory(const std::vector<std::byte> &payload, std::size_t offset = 0);

} // namespace TerritoryDSI
