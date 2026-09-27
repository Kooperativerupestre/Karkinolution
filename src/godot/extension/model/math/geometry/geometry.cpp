#include "godot/extension/model/math/geometry/geometry.hpp"

// GodotRadius

void GodotRadius::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotRadius::get_value);
	godot::ClassDB::bind_static_method("GodotRadius",
									   godot::D_METHOD("create", "value"),
									   &GodotRadius::create,
									   DEFVAL(0.0));
}

GodotRadius::GodotRadius()
	: value_(0.0) {}

GodotRadius::GodotRadius(double value)
	: value_(value) {}

double GodotRadius::get_value() const {
	return value_;
}

godot::Ref<GodotRadius> GodotRadius::create(double value) {
	godot::Ref<GodotRadius> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotCircumference

void GodotCircumference::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotCircumference::get_value);
	godot::ClassDB::bind_static_method("GodotCircumference",
									   godot::D_METHOD("create", "value"),
									   &GodotCircumference::create,
									   DEFVAL(0.0));
}

GodotCircumference::GodotCircumference()
	: value_(0.0) {}

GodotCircumference::GodotCircumference(double value)
	: value_(value) {}

double GodotCircumference::get_value() const {
	return value_;
}

godot::Ref<GodotCircumference> GodotCircumference::create(double value) {
	godot::Ref<GodotCircumference> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotDiameter

void GodotDiameter::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotDiameter::get_value);
	godot::ClassDB::bind_static_method("GodotDiameter",
									   godot::D_METHOD("create", "value"),
									   &GodotDiameter::create,
									   DEFVAL(0.0));
}

GodotDiameter::GodotDiameter()
	: value_(0.0) {}

GodotDiameter::GodotDiameter(double value)
	: value_(value) {}

double GodotDiameter::get_value() const {
	return value_;
}

godot::Ref<GodotDiameter> GodotDiameter::create(double value) {
	godot::Ref<GodotDiameter> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotArea

void GodotArea::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotArea::get_value);
	godot::ClassDB::bind_static_method("GodotArea",
									   godot::D_METHOD("create", "value"),
									   &GodotArea::create,
									   DEFVAL(0.0));
}

GodotArea::GodotArea()
	: value_(0.0) {}

GodotArea::GodotArea(double value)
	: value_(value) {}

double GodotArea::get_value() const {
	return value_;
}

godot::Ref<GodotArea> GodotArea::create(double value) {
	godot::Ref<GodotArea> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}
