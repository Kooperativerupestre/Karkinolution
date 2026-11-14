#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Territory {
	ResponseFrame see_territory(const Karkinolution::World::World &world, Binary::RequestId request_id);
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Territory
