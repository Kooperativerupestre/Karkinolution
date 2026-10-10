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
};