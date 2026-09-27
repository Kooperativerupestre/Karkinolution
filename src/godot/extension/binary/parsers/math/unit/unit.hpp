#pragma once

#include "godot/extension/binary/parsers/byte_utils.hpp"
#include "godot/extension/model/math/unit/units.hpp"

#include <cstddef>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp>

namespace GodotBinaryParser {

	inline godot::Ref<GodotVolume> parse_volume(const godot::PackedByteArray &payload,
												std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_volume(to_bytes(payload), offset);
		return GodotVolume::create(core_val.value);
	}

	inline godot::Ref<GodotMass> parse_mass(const godot::PackedByteArray &payload,
											std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_mass(to_bytes(payload), offset);
		return GodotMass::create(core_val.value);
	}

	inline godot::Ref<GodotDensity> parse_density(const godot::PackedByteArray &payload,
												  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_density(to_bytes(payload), offset);
		return GodotDensity::create(core_val.value);
	}

	inline godot::Ref<GodotMeter> parse_meter(const godot::PackedByteArray &payload,
											  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_meter(to_bytes(payload), offset);
		return GodotMeter::create(core_val.value);
	}

	inline godot::Ref<GodotLateral> parse_lateral(const godot::PackedByteArray &payload,
												  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_lateral(to_bytes(payload), offset);
		return GodotLateral::create(core_val.value);
	}

	inline godot::Ref<GodotHeight> parse_height(const godot::PackedByteArray &payload,
												std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_height(to_bytes(payload), offset);
		return GodotHeight::create(core_val.value);
	}

	inline godot::Ref<GodotDepth> parse_depth(const godot::PackedByteArray &payload,
											  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_depth(to_bytes(payload), offset);
		return GodotDepth::create(core_val.value);
	}

	inline godot::Ref<GodotSize> parse_size(const godot::PackedByteArray &payload,
											std::size_t                   offset = 0) {
		const auto core_size = PhysicsUnitsDSI::deserialize_size(to_bytes(payload), offset);
		return GodotSize::from_values(core_size.lateral.value,
									  core_size.height.value,
									  core_size.depth.value);
	}

} // namespace GodotBinaryParser
