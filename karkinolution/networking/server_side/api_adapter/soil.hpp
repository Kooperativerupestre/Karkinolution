#pragma once

#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/protocol/aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Soil {
	ResponseFrame see_soil(const Karkinolution::World::World &world,
						   Frames::World::RequestId           request_id,
						   const std::vector<std::byte>      &payload);
	ResponseFrame tick_soil(Karkinolution::World::World  &world,
							Frames::World::RequestId      request_id,
							const std::vector<std::byte> &payload);
	ResponseFrame erase_soil(Karkinolution::World::World  &world,
							 Frames::World::RequestId      request_id,
							 const std::vector<std::byte> &payload);
	ResponseFrame see_all_soils(const Karkinolution::World::World &world,
								Frames::World::RequestId           request_id,
								const std::vector<std::byte>      &payload);
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Soil
