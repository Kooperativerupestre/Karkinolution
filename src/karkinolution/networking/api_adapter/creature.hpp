#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace CreatureAPIBYA {
	ResponseFrame
	see_creature(const World &world, RequestId request_id, const std::vector<std::byte> &payload);
} // namespace CreatureAPIBYA
