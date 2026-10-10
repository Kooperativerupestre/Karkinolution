#include "godot/extension/model/corpse.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>

using KarkinolutionGodot::Models::Organisms::Corpse::GodotCorpse;

void GodotCorpse::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_id"), &GodotCorpse::get_id);
	godot::ClassDB::bind_method(godot::D_METHOD("get_raw_meat"), &GodotCorpse::get_raw_meat);
	godot::ClassDB::bind_method(godot::D_METHOD("get_position"), &GodotCorpse::get_position);
	godot::ClassDB::bind_method(godot::D_METHOD("get_size"), &GodotCorpse::get_size);
}

GodotCorpse::GodotCorpse(std::int64_t                 id,
						 float                        raw_meat,
						 const godot::Vector3        &position,
						 const godot::Ref<GodotSize> &size)
	: id_(id)
	, raw_meat_(raw_meat)
	, position_(position)
	, size_(size) {}

std::int64_t GodotCorpse::get_id() const {
	return id_;
}

float GodotCorpse::get_raw_meat() const {
	return raw_meat_;
}

godot::Vector3 GodotCorpse::get_position() const {
	return position_;
}

godot::Ref<GodotSize> GodotCorpse::get_size() const {
	return size_;
}
