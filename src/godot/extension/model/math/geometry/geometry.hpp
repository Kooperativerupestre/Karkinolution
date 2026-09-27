#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

class GodotRadius : public godot::RefCounted {
		GDCLASS(GodotRadius, godot::RefCounted);

	protected:

		double value_{0.0};

		static void _bind_methods();

	public:

		GodotRadius();
		explicit GodotRadius(double value);

		[[nodiscard]] double get_value() const;

		static godot::Ref<GodotRadius> create(double value = 0.0);
};

class GodotCircumference : public godot::RefCounted {
		GDCLASS(GodotCircumference, godot::RefCounted);

	protected:

		double value_{0.0};

		static void _bind_methods();

	public:

		GodotCircumference();
		explicit GodotCircumference(double value);

		[[nodiscard]] double get_value() const;

		static godot::Ref<GodotCircumference> create(double value = 0.0);
};

class GodotDiameter : public godot::RefCounted {
		GDCLASS(GodotDiameter, godot::RefCounted);

	protected:

		double value_{0.0};

		static void _bind_methods();

	public:

		GodotDiameter();
		explicit GodotDiameter(double value);

		[[nodiscard]] double get_value() const;

		static godot::Ref<GodotDiameter> create(double value = 0.0);
};

class GodotArea : public godot::RefCounted {
		GDCLASS(GodotArea, godot::RefCounted);

	protected:

		double value_{0.0};

		static void _bind_methods();

	public:

		GodotArea();
		explicit GodotArea(double value);

		[[nodiscard]] double get_value() const;

		static godot::Ref<GodotArea> create(double value = 0.0);
};
