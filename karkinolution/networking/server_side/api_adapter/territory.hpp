#pragma once

#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Territory {
	ResponseFrame see_territory(const Karkinolution::World::World &world, Frames::World::RequestId request_id);
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Territory
