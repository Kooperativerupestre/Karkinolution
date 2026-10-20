#pragma once

#include "karkinolution/binary/frames/aliases.hpp"

#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/world/world.hpp>

namespace CorpseAPIBYA {
	ResponseFrame
	see_corpse(const World &world, RequestId request_id, const std::vector<std::byte> &payload);
} // namespace CorpseAPIBYA