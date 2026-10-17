#pragma once

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

namespace KarkinolutionGodot {
	namespace Models {
		namespace Properties {

			class GodotProperty : public godot::RefCounted {
					GDCLASS(GodotProperty, godot::RefCounted);

				private:

					std::uint8_t value_;

				protected:

					static void _bind_methods();

				public:

					GodotProperty();
					GodotProperty(std::uint8_t value);

					[[nodiscard]] std::uint8_t get_value() const;

					void set_value(std::uint8_t value) {
						value_ = value;
					}
			};
		} // namespace Properties
	} // namespace Models
} // namespace KarkinolutionGodot