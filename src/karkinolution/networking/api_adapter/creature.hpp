#pragma once
#include <karkinolution/api/creature.hpp>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature.hpp>

namespace CreatureAPIBYA {
	std::vector<std::byte> get_creature(const World &world, const std::vector<std::byte> &request);
} // namespace CreatureAPIBYA
