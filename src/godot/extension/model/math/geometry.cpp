#include "godot/extension/model/math/geometry.hpp"

// GodotRadius

void GodotRadius::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotRadius::get_value);
}

GodotRadius::GodotRadius() {}

GodotRadius::GodotRadius(double value)
	: value_(value) {}

double GodotRadius::get_value() const {
	return value_;
}

// GodotCircumference

void GodotCircumference::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotCircumference::get_value);
}

GodotCircumference::GodotCircumference() {}

GodotCircumference::GodotCircumference(double value)
	: value_(value) {}

double GodotCircumference::get_value() const {
	return value_;
}

// GodotDiameter

void GodotDiameter::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotDiameter::get_value);
}

GodotDiameter::GodotDiameter() {}

GodotDiameter::GodotDiameter(double value)
	: value_(value) {}

double GodotDiameter::get_value() const {
	return value_;
}

// GodotArea

void GodotArea::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotArea::get_value);
}

GodotArea::GodotArea() {}

GodotArea::GodotArea(double value)
	: value_(value) {}

double GodotArea::get_value() const {
	return value_;
}
