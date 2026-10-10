#pragma once
#include "karkinolution/binary/frames/parser.hpp"

#include <karkinolution/api/creature.hpp>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature/alias.hpp>
#include <karkinolution/binary/frames/aliases.hpp>

namespace CreatureAPIBYA {
	ResponseFrame see_creature(const World &world, const std::vector<std::byte> &payload);
} // namespace CreatureAPIBYA
