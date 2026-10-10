#include <godot/extension/model/properties.hpp>

using KarkinolutionGodot::Models::Properties::GodotProperty;

void GodotProperty::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotProperty::get_value);
}

[[nodiscard]] std::uint8_t GodotProperty::get_value() const {
	return value_;
}