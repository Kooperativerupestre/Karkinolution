#include "karkinolution.hpp"

#include "binary/parser.hpp"

#include <godot_cpp/core/class_db.hpp>

void Karkinolution::_bind_methods() {
	godot::ClassDB::bind_static_method("Karkinolution",
									   godot::D_METHOD("parse_frame", "bytes"),
									   &Karkinolution::parse_frame);
	godot::ClassDB::bind_static_method("Karkinolution",
									   godot::D_METHOD("parse_creature", "payload", "id"),
									   &Karkinolution::parse_creature,
									   DEFVAL(0));
	godot::ClassDB::bind_static_method("Karkinolution",
									   godot::D_METHOD("build_get_creature_request", "id"),
									   &Karkinolution::build_get_creature_request);
}

godot::Ref<GodotParsedFrame> Karkinolution::parse_frame(const godot::PackedByteArray &bytes) {
	return GodotBinaryParser::parse_frame(bytes);
}

godot::Ref<GodotCreature> Karkinolution::parse_creature(const godot::PackedByteArray &payload,
													   std::uint64_t                id) {
	return GodotBinaryParser::parse_creature(payload, id);
}

godot::PackedByteArray Karkinolution::build_get_creature_request(std::uint64_t id) {
	return GodotBinaryParser::build_get_creature_request(id);
}