#pragma once

#include <godot/extension/utils/int64_conversor.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

/*
 * Architectural Decision:
 * LimitedValue, NormalizedValue, and SignedNormalizedValue collapse into GodotLimitedValue
 * with runtime value, min, and max properties to avoid template proliferation in ClassDB
 * and provide a simple scalar transport struct for GDScript.


 And Godot has only int and float. So, each variant of int, collapses into IntLimitedValue and each
 variant of float collapses into DoubleLimitedValue
 */


namespace KarkinolutionGodot {
	namespace Models {
		namespace Math {
			namespace Stats {
				class DoubleLimitedValue : public godot::RefCounted {
						GDCLASS(DoubleLimitedValue, godot::RefCounted);

						double value_{0.0};
						double min_{0.0};
						double max_{0.0};

					protected:

						static void _bind_methods();

					public:

						DoubleLimitedValue() = default;

						template <std::floating_point T>
						DoubleLimitedValue(T value, T max, T min)
							: value_(KarkinolutionGodot::to_int64(value))
							, max_(KarkinolutionGodot::to_int64(value))
							, min_(KarkinolutionGodot::to_int64(value)) {}

						[[nodiscard]] double get_value() const;
						[[nodiscard]] double get_min() const;
						[[nodiscard]] double get_max() const;

						void set_value(double value) {
							value_ = value;
						}

						void set_min(double min) {
							min_ = min;
						}

						void set_max(double max) {
							max_ = max;
						}
				};

				class IntLimitedValue : public godot::RefCounted {
						GDCLASS(IntLimitedValue, godot::RefCounted);

						int64_t value_{0}, max_{0}, min_{0};

					protected:

						static void _bind_methods();

					public:

						IntLimitedValue() = default;


						template <std::integral T>
						IntLimitedValue(T value, T max, T min)
							: value_(static_cast<double>(value))
							, max_(static_cast<double>(value))
							, min_(static_cast<double>(value)) {}

						[[nodiscard]] int64_t get_value() const;
						[[nodiscard]] int64_t get_min() const;
						[[nodiscard]] int64_t get_max() const;

						void set_value(int64_t value) {
							value_ = value;
						}

						void set_min(int64_t min) {
							min_ = min;
						}

						void set_max(int64_t max) {
							max_ = max;
						}
				};
			} // namespace Stats
		} // namespace Math
	} // namespace Models
} // namespace KarkinolutionGodot
