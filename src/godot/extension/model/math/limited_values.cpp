#include "godot/extension/model/math/limited_values.hpp"

#include <godot/extension/model/math/aliases.hpp>
#include <godot/extension/utils/int64_conversor.hpp>

void DoubleLimitedValue::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &DoubleLimitedValue::get_value);
	godot::ClassDB::bind_method(godot::D_METHOD("get_min"), &DoubleLimitedValue::get_min);
	godot::ClassDB::bind_method(godot::D_METHOD("get_max"), &DoubleLimitedValue::get_max);
}

double DoubleLimitedValue::get_value() const {
	return value_;
}

double DoubleLimitedValue::get_min() const {
	return min_;
}

double DoubleLimitedValue::get_max() const {
	return max_;
}

void IntLimitedValue::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_value"), &IntLimitedValue::get_value);
	godot::ClassDB::bind_method(godot::D_METHOD("get_max"), &IntLimitedValue::get_max);
	godot::ClassDB::bind_method(godot::D_METHOD("get_min"), &IntLimitedValue::get_min);
}

int64_t IntLimitedValue::get_value() const {
	return value_;
}

int64_t IntLimitedValue::get_max() const {
	return max_;
}

int64_t IntLimitedValue::get_min() const {
	return min_;
}