#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Soil {
	ResponseFrame see_soil(const Karkinolution::World::World &world,
						   Binary::RequestId                  request_id,
						   const std::vector<std::byte>      &payload);
	ResponseFrame see_all_soils(const Karkinolution::World::World &world,
								Binary::RequestId                  request_id,
								const std::vector<std::byte>      &payload);
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Soil
