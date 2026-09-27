#include "godot/extension/model/math/stats/limited_value.hpp"

void GodotLimitedValue::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &GodotLimitedValue::get_value);
	godot::ClassDB::bind_method(godot::D_METHOD("get_min"), &GodotLimitedValue::get_min);
	godot::ClassDB::bind_method(godot::D_METHOD("get_max"), &GodotLimitedValue::get_max);
	godot::ClassDB::bind_method(godot::D_METHOD("get_ratio"), &GodotLimitedValue::get_ratio);
	godot::ClassDB::bind_method(godot::D_METHOD("get_ratio_min"),
								&GodotLimitedValue::get_ratio_min);
	godot::ClassDB::bind_method(godot::D_METHOD("get_remaining"),
								&GodotLimitedValue::get_remaining);
	godot::ClassDB::bind_method(godot::D_METHOD("is_full"), &GodotLimitedValue::is_full);
	godot::ClassDB::bind_method(godot::D_METHOD("is_zero"), &GodotLimitedValue::is_zero);

	godot::ClassDB::bind_static_method("GodotLimitedValue",
									   godot::D_METHOD("create", "value", "min", "max"),
									   &GodotLimitedValue::create,
									   DEFVAL(0.0),
									   DEFVAL(0.0),
									   DEFVAL(1.0));
}

GodotLimitedValue::GodotLimitedValue()
	: value_(0.0)
	, min_(0.0)
	, max_(1.0) {}

GodotLimitedValue::GodotLimitedValue(double value, double min, double max)
	: value_(value)
	, min_(min)
	, max_(max) {}

double GodotLimitedValue::get_value() const {
	return value_;
}

double GodotLimitedValue::get_min() const {
	return min_;
}

double GodotLimitedValue::get_max() const {
	return max_;
}

double GodotLimitedValue::get_ratio() const {
	const double range = max_ - min_;
	if (range <= 0.0) {
		return 0.0;
	}
	return (value_ - min_) / range;
}

double GodotLimitedValue::get_ratio_min() const {
	return get_ratio() * 2.0 - 1.0;
}

double GodotLimitedValue::get_remaining() const {
	return max_ - value_;
}

bool GodotLimitedValue::is_full() const {
	return value_ >= max_;
}

bool GodotLimitedValue::is_zero() const {
	return value_ <= 0.0;
}

godot::Ref<GodotLimitedValue> GodotLimitedValue::create(double value, double min, double max) {
	godot::Ref<GodotLimitedValue> ref;
	ref.instantiate();
	ref->value_ = value;
	ref->min_   = min;
	ref->max_   = max;
	return ref;
}
