#pragma once

#include "karkinolution/binary/frames/aliases.hpp"

#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::APIBYA::Corpse {
	ResponseFrame
	see_corpse(const Karkinolution::World::World &world, Binary::RequestId request_id, const std::vector<std::byte> &payload);
} // namespace Karkinolution::Networking::APIBYA::Corpse
