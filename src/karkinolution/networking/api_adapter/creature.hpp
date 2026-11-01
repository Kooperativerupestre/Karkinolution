#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::APIBYA::Creature {
	ResponseFrame
	see_creature(const Karkinolution::World::World &world, RequestId request_id, const std::vector<std::byte> &payload);
	ResponseFrame
	create_creature(Karkinolution::World::World &world, RequestId request_id, const std::vector<std::byte> &payload);
} // namespace Karkinolution::Networking::APIBYA::Creature
