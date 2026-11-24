#pragma once

#include "karkinolution/networking/frame/world/aliases.hpp"

#include <karkinolution/networking/frame/world/core_aliases.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Corpse {
	ResponseFrame see_corpse(const Karkinolution::World::World &world,
							 Frames::World::RequestId           request_id,
							 const std::vector<std::byte>      &payload);
	ResponseFrame tick_corpse(Karkinolution::World::World  &world,
							  Frames::World::RequestId      request_id,
							  const std::vector<std::byte> &payload);
	ResponseFrame erase_corpse(Karkinolution::World::World  &world,
							   Frames::World::RequestId      request_id,
							   const std::vector<std::byte> &payload);
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Corpse
