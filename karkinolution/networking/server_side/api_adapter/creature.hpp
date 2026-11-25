#pragma once

#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Creature {
	ResponseFrame see_creature(const Karkinolution::World::World &world,
							   Frames::World::RequestId           request_id,
							   const std::vector<std::byte>      &payload);
	ResponseFrame create_creature(Karkinolution::World::World  &world,
								  Frames::World::RequestId      request_id,
								  const std::vector<std::byte> &payload);
	ResponseFrame see_all_creatures(const Karkinolution::World::World &world, Frames::World::RequestId request_id);
	ResponseFrame tick_creature(Karkinolution::World::World  &world,
								Frames::World::RequestId      request_id,
								const std::vector<std::byte> &payload);
	ResponseFrame erase_creature(Karkinolution::World::World  &world,
								 Frames::World::RequestId      request_id,
								 const std::vector<std::byte> &payload);
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Creature
