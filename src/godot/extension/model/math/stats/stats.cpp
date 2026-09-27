#include "godot/extension/model/math/stats/stats.hpp"

// GodotSharedVolume

void GodotSharedVolume::_bind_methods() {
	godot::ClassDB::bind_static_method("GodotSharedVolume",
									   godot::D_METHOD("create", "value"),
									   &GodotSharedVolume::create,
									   DEFVAL(0.0));
}

GodotSharedVolume::GodotSharedVolume()
	: GodotLimitedValue(0.0, 0.0, 1.0) {}

GodotSharedVolume::GodotSharedVolume(double value)
	: GodotLimitedValue(value, 0.0, 1.0) {}

godot::Ref<GodotSharedVolume> GodotSharedVolume::create(double value) {
	godot::Ref<GodotSharedVolume> ref;
	ref.instantiate();
	ref->value_ = value;
	ref->min_   = 0.0;
	ref->max_   = 1.0;
	return ref;
}

// GodotEfficiency

void GodotEfficiency::_bind_methods() {
	godot::ClassDB::bind_static_method("GodotEfficiency",
									   godot::D_METHOD("create", "value"),
									   &GodotEfficiency::create,
									   DEFVAL(0.0));
}

GodotEfficiency::GodotEfficiency()
	: GodotLimitedValue(0.0, 0.0, 1.0) {}

GodotEfficiency::GodotEfficiency(double value)
	: GodotLimitedValue(value, 0.0, 1.0) {}

godot::Ref<GodotEfficiency> GodotEfficiency::create(double value) {
	godot::Ref<GodotEfficiency> ref;
	ref.instantiate();
	ref->value_ = value;
	ref->min_   = 0.0;
	ref->max_   = 1.0;
	return ref;
}

// GodotQuality

void GodotQuality::_bind_methods() {
	godot::ClassDB::bind_static_method("GodotQuality",
									   godot::D_METHOD("create", "value"),
									   &GodotQuality::create,
									   DEFVAL(0.0));
}

GodotQuality::GodotQuality()
	: GodotLimitedValue(0.0, 0.0, 1.0) {}

GodotQuality::GodotQuality(double value)
	: GodotLimitedValue(value, 0.0, 1.0) {}

godot::Ref<GodotQuality> GodotQuality::create(double value) {
	godot::Ref<GodotQuality> ref;
	ref.instantiate();
	ref->value_ = value;
	ref->min_   = 0.0;
	ref->max_   = 1.0;
	return ref;
}
