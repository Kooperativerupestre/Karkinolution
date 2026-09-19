#pragma once

#include "binary/frame.hpp"
#include "model/creature/creature.hpp"

#include <cstdint>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

class Karkinolution : public godot::Node {
		GDCLASS(Karkinolution, godot::Node);

	protected:

		static void _bind_methods();

	public:

		static godot::Ref<GodotParsedFrame> parse_frame(const godot::PackedByteArray &bytes);
		static godot::Ref<GodotCreature>    parse_creature(const godot::PackedByteArray &payload,
														   std::uint64_t                 id = 0);
		static godot::PackedByteArray       build_get_creature_request(std::uint64_t id);
};