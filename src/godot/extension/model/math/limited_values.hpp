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

						double value_;
						double min_;
						double max_;

						DoubleLimitedValue() {}

					protected:


						static void _bind_methods();

					public:


						template <std::floating_point T>
						DoubleLimitedValue(T value, T max, T min)
							: value_(KarkinolutionGodot::to_int64(value))
							, max_(KarkinolutionGodot::to_int64(value))
							, min_(KarkinolutionGodot::to_int64(value)) {}

						[[nodiscard]] double get_value() const;
						[[nodiscard]] double get_min() const;
						[[nodiscard]] double get_max() const;
				};

				class IntLimitedValue : public godot::RefCounted {
						GDCLASS(IntLimitedValue, godot::RefCounted);

						int64_t value_, max_, min_;

					private:

						IntLimitedValue() {}

						static void _bind_methods();

					public:


						template <std::integral T>
						IntLimitedValue(T value, T max, T min)
							: value_(static_cast<double>(value))
							, max_(static_cast<double>(value))
							, min_(static_cast<double>(value)) {}

						[[nodiscard]] int64_t get_value() const;
						[[nodiscard]] int64_t get_min() const;
						[[nodiscard]] int64_t get_max() const;
				};
			} // namespace Stats
		} // namespace Math
	} // namespace Models
} // namespace KarkinolutionGodot
