#include "godot/extension/model/properties/properties.hpp"

void GodotGenericProperty::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_move_capability"),
								&GodotGenericProperty::get_move_capability);
	godot::ClassDB::bind_method(godot::D_METHOD("get_move_capability_name"),
								&GodotGenericProperty::get_move_capability_name);

	godot::ClassDB::bind_static_method("GodotGenericProperty",
									   godot::D_METHOD("create", "move_capability"),
									   &GodotGenericProperty::create,
									   DEFVAL(0));

	BIND_ENUM_CONSTANT(WALK);
	BIND_ENUM_CONSTANT(SWIMM);
}

GodotGenericProperty::GodotGenericProperty()
	: move_capability_(0) {}

GodotGenericProperty::GodotGenericProperty(std::uint8_t move_capability)
	: move_capability_(move_capability) {}

std::uint8_t GodotGenericProperty::get_move_capability() const {
	return move_capability_;
}

godot::String GodotGenericProperty::get_move_capability_name() const {
	switch (move_capability_) {
		case WALK:
			return "WALK";
		case SWIMM:
			return "SWIMM";
		default:
			return "UNKNOWN";
	}
}

godot::Ref<GodotGenericProperty> GodotGenericProperty::create(std::uint8_t move_capability) {
	godot::Ref<GodotGenericProperty> ref;
	ref.instantiate();
	ref->move_capability_ = move_capability;
	return ref;
}
