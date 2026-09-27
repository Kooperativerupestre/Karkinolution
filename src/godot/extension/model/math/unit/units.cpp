#include "godot/extension/model/math/unit/units.hpp"

// GodotMeter

void GodotMeter::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotMeter::get_value);
	godot::ClassDB::bind_static_method("GodotMeter",
									   godot::D_METHOD("create", "value"),
									   &GodotMeter::create,
									   DEFVAL(0.0));
}

GodotMeter::GodotMeter() {}

GodotMeter::GodotMeter(double value)
	: value_(value) {}

double GodotMeter::get_value() const {
	return value_;
}

godot::Ref<GodotMeter> GodotMeter::create(double value) {
	godot::Ref<GodotMeter> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotLateral

void GodotLateral::_bind_methods() {
	godot::ClassDB::bind_static_method("GodotLateral",
									   godot::D_METHOD("create", "value"),
									   &GodotLateral::create,
									   DEFVAL(0.0));
}

godot::Ref<GodotLateral> GodotLateral::create(double value) {
	godot::Ref<GodotLateral> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotHeight

void GodotHeight::_bind_methods() {
	godot::ClassDB::bind_static_method("GodotHeight",
									   godot::D_METHOD("create", "value"),
									   &GodotHeight::create,
									   DEFVAL(0.0));
}

godot::Ref<GodotHeight> GodotHeight::create(double value) {
	godot::Ref<GodotHeight> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotDepth

void GodotDepth::_bind_methods() {
	godot::ClassDB::bind_static_method("GodotDepth",
									   godot::D_METHOD("create", "value"),
									   &GodotDepth::create,
									   DEFVAL(0.0));
}

godot::Ref<GodotDepth> GodotDepth::create(double value) {
	godot::Ref<GodotDepth> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotVolume

void GodotVolume::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotVolume::get_value);
	godot::ClassDB::bind_static_method("GodotVolume",
									   godot::D_METHOD("create", "value"),
									   &GodotVolume::create,
									   DEFVAL(0.0));
}

GodotVolume::GodotVolume() {}

GodotVolume::GodotVolume(double value)
	: value_(value) {}

double GodotVolume::get_value() const {
	return value_;
}

godot::Ref<GodotVolume> GodotVolume::create(double value) {
	godot::Ref<GodotVolume> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotMass

void GodotMass::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotMass::get_value);
	godot::ClassDB::bind_static_method("GodotMass",
									   godot::D_METHOD("create", "value"),
									   &GodotMass::create,
									   DEFVAL(0.0));
}

GodotMass::GodotMass()
	: value_(0.0) {}

GodotMass::GodotMass(double value)
	: value_(value) {}

double GodotMass::get_value() const {
	return value_;
}

godot::Ref<GodotMass> GodotMass::create(double value) {
	godot::Ref<GodotMass> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotDensity

void GodotDensity::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotDensity::get_value);
	godot::ClassDB::bind_static_method("GodotDensity",
									   godot::D_METHOD("create", "value"),
									   &GodotDensity::create,
									   DEFVAL(0.0));
}

GodotDensity::GodotDensity()
	: value_(0.0) {}

GodotDensity::GodotDensity(double value)
	: value_(value) {}

double GodotDensity::get_value() const {
	return value_;
}

godot::Ref<GodotDensity> GodotDensity::create(double value) {
	godot::Ref<GodotDensity> ref;
	ref.instantiate();
	ref->value_ = value;
	return ref;
}

// GodotSize

void GodotSize::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_lateral"), &GodotSize::get_lateral);
	godot::ClassDB::bind_method(godot::D_METHOD("get_height"), &GodotSize::get_height);
	godot::ClassDB::bind_method(godot::D_METHOD("get_depth"), &GodotSize::get_depth);
	godot::ClassDB::bind_method(godot::D_METHOD("get_volume"), &GodotSize::get_volume);

	godot::ClassDB::bind_static_method("GodotSize",
									   godot::D_METHOD("create", "lateral", "height", "depth"),
									   &GodotSize::create,
									   DEFVAL(godot::Ref<GodotLateral>()),
									   DEFVAL(godot::Ref<GodotHeight>()),
									   DEFVAL(godot::Ref<GodotDepth>()));

	godot::ClassDB::bind_static_method("GodotSize",
									   godot::D_METHOD("from_values", "lateral", "height", "depth"),
									   &GodotSize::from_values,
									   DEFVAL(0.0),
									   DEFVAL(0.0),
									   DEFVAL(0.0));
}

GodotSize::GodotSize()
	: lateral_(GodotLateral::create(0.0))
	, height_(GodotHeight::create(0.0))
	, depth_(GodotDepth::create(0.0)) {}

GodotSize::GodotSize(const godot::Ref<GodotLateral> &lateral,
					 const godot::Ref<GodotHeight>  &height,
					 const godot::Ref<GodotDepth>   &depth)
	: lateral_(lateral.is_valid() ? lateral : GodotLateral::create(0.0))
	, height_(height.is_valid() ? height : GodotHeight::create(0.0))
	, depth_(depth.is_valid() ? depth : GodotDepth::create(0.0)) {}

godot::Ref<GodotLateral> GodotSize::get_lateral() const {
	return lateral_;
}

godot::Ref<GodotHeight> GodotSize::get_height() const {
	return height_;
}

godot::Ref<GodotDepth> GodotSize::get_depth() const {
	return depth_;
}

godot::Ref<GodotVolume> GodotSize::get_volume() const {
	const double lat = lateral_.is_valid() ? lateral_->get_value() : 0.0;
	const double h   = height_.is_valid() ? height_->get_value() : 0.0;
	const double d   = depth_.is_valid() ? depth_->get_value() : 0.0;
	return GodotVolume::create(lat * h * d);
}

godot::Ref<GodotSize> GodotSize::create(const godot::Ref<GodotLateral> &lateral,
										const godot::Ref<GodotHeight>  &height,
										const godot::Ref<GodotDepth>   &depth) {
	godot::Ref<GodotSize> ref;
	ref.instantiate();
	ref->lateral_ = lateral.is_valid() ? lateral : GodotLateral::create(0.0);
	ref->height_  = height.is_valid() ? height : GodotHeight::create(0.0);
	ref->depth_   = depth.is_valid() ? depth : GodotDepth::create(0.0);
	return ref;
}

godot::Ref<GodotSize> GodotSize::from_values(double lateral, double height, double depth) {
	return create(GodotLateral::create(lateral),
				  GodotHeight::create(height),
				  GodotDepth::create(depth));
}
