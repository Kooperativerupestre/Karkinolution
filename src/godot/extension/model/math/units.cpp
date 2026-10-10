#include "godot/extension/model/math/units.hpp"

#include <godot/extension/model/math/aliases.hpp>

// GodotMeter

void GodotMeter::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotMeter::get_value);
}

GodotMeter::GodotMeter() {}

GodotMeter::GodotMeter(double value)
	: value_(value) {}

double GodotMeter::get_value() const {
	return value_;
}

// GodotLateral

void GodotLateral::_bind_methods() {}

// GodotHeight

void GodotHeight::_bind_methods() {}

// GodotDepth

void GodotDepth::_bind_methods() {}

// GodotVolume

void GodotVolume::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotVolume::get_value);
}

GodotVolume::GodotVolume() {}

GodotVolume::GodotVolume(double value)
	: value_(value) {}

double GodotVolume::get_value() const {
	return value_;
}

// GodotMass

void GodotMass::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotMass::get_value);
}

GodotMass::GodotMass() {}

GodotMass::GodotMass(double value)
	: value_(value) {}

double GodotMass::get_value() const {
	return value_;
}

// GodotDensity

void GodotDensity::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotDensity::get_value);
}

GodotDensity::GodotDensity() {}

GodotDensity::GodotDensity(double value)
	: value_(value) {}

double GodotDensity::get_value() const {
	return value_;
}

// GodotSize

void GodotSize::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_lateral"), &GodotSize::get_lateral);
	godot::ClassDB::bind_method(godot::D_METHOD("get_height"), &GodotSize::get_height);
	godot::ClassDB::bind_method(godot::D_METHOD("get_depth"), &GodotSize::get_depth);
	godot::ClassDB::bind_method(godot::D_METHOD("get_volume"), &GodotSize::get_volume);
}

GodotSize::GodotSize() {
	lateral_.instantiate();
	height_.instantiate();
	depth_.instantiate();
}

GodotSize::GodotSize(double lateral, double height, double depth)
	: lateral_(lateral)
	, height_(height)
	, depth_(depth) {}

double GodotSize::get_lateral() const {
	return lateral_->get_value();
}

double GodotSize::get_height() const {
	return height_->get_value();
}

double GodotSize::get_depth() const {
	return depth_->get_value();
}

double GodotSize::get_volume() const {
	const double lat = lateral_.is_valid() ? lateral_->get_value() : 0.0;
	const double h   = height_.is_valid() ? height_->get_value() : 0.0;
	const double d   = depth_.is_valid() ? depth_->get_value() : 0.0;
	return lat * h * d;
}
