#pragma once

#include <cstddef>
#include <cstdint>
#include <karkinolution/binary/deserialization/interpreters/models.hpp>
#include <karkinolution/core/id.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/stats.hpp>
#include <vector>

namespace CorpseResponseDSI {
	std::uint64_t      get_id(const std::vector<std::byte> &payload, std::size_t offset = 0);
	RawMeat            get_raw_meat(const std::vector<std::byte> &payload, std::size_t offset = 0);
	Vec3               get_position(const std::vector<std::byte> &payload, std::size_t offset = 0);
	Size               get_size(const std::vector<std::byte> &payload, std::size_t offset = 0);
	DesserializedCorpse get_corpse(const std::vector<std::byte> &payload, std::size_t offset = 0);
} // namespace CorpseResponseDSI

namespace CorpseRequestDSI {
	BaseIdType interpret_like_get_corpse(const std::vector<std::byte> &payload,
										 std::size_t                   offset = 0);
} // namespace CorpseRequestDSI
