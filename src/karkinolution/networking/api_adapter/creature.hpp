#pragma once
#include "karkinolution/binary/frames/parser.hpp"

#include <karkinolution/api/creature.hpp>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/deserializer.hpp>
#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>

namespace CreatureAPIBYA {
	ResponseFrame
	see_creature(const World &world, RequestId request_id, const std::vector<std::byte> &payload);
} // namespace CreatureAPIBYA
