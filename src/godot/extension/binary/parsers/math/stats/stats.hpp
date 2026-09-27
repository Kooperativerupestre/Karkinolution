#pragma once

#include "binary/parsers/byte_utils.hpp"
#include "godot/extension/model/math/stats/limited_value.hpp"
#include "godot/extension/model/math/stats/stats.hpp"

#include <cstddef>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp>

namespace GodotBinaryParser {

	inline godot::Ref<GodotSharedVolume> parse_shared_volume(const godot::PackedByteArray &payload,
															 std::size_t offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_shared_volume(to_bytes(payload), offset);
		return GodotSharedVolume::create(core_val.value());
	}

	inline godot::Ref<GodotEfficiency> parse_efficiency(const godot::PackedByteArray &payload,
														std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_efficiency(to_bytes(payload), offset);
		return GodotEfficiency::create(static_cast<double>(core_val.value()));
	}

	inline godot::Ref<GodotQuality> parse_quality(const godot::PackedByteArray &payload,
												  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_quality(to_bytes(payload), offset);
		return GodotQuality::create(static_cast<double>(core_val.value()));
	}

} // namespace GodotBinaryParser
