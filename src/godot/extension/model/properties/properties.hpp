#pragma once

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

class GodotGenericProperty : public godot::RefCounted {
		GDCLASS(GodotGenericProperty, godot::RefCounted);

	public:

		enum MoveCapability {
			WALK = 0,
			SWIMM = 1
		};

	protected:

		std::uint8_t move_capability_{0};

		static void _bind_methods();

	public:

		GodotGenericProperty();
		explicit GodotGenericProperty(std::uint8_t move_capability);

		[[nodiscard]] std::uint8_t  get_move_capability() const;
		[[nodiscard]] godot::String get_move_capability_name() const;

		static godot::Ref<GodotGenericProperty> create(std::uint8_t move_capability = 0);
};

VARIANT_ENUM_CAST(GodotGenericProperty::MoveCapability);
