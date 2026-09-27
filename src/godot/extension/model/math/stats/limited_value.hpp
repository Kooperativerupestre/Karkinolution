#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

/*
 * Architectural Decision:
 * LimitedValue, NormalizedValue, and SignedNormalizedValue collapse into GodotLimitedValue
 * with runtime value, min, and max properties to avoid template proliferation in ClassDB
 * and provide a simple scalar transport struct for GDScript.
 */

class GodotLimitedValue : public godot::RefCounted {
		GDCLASS(GodotLimitedValue, godot::RefCounted);

	protected:

		double value_{0.0};
		double min_{0.0};
		double max_{1.0};

		static void _bind_methods();

	public:

		GodotLimitedValue();
		GodotLimitedValue(double value, double min, double max);

		[[nodiscard]] double get_value() const;
		[[nodiscard]] double get_min() const;
		[[nodiscard]] double get_max() const;

		[[nodiscard]] double get_ratio() const;
		[[nodiscard]] double get_ratio_min() const;
		[[nodiscard]] double get_remaining() const;

		[[nodiscard]] bool is_full() const;
		[[nodiscard]] bool is_zero() const;

		static godot::Ref<GodotLimitedValue> create(double value = 0.0,
													double min   = 0.0,
													double max   = 1.0);
};
