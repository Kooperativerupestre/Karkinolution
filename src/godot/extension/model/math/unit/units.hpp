#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

class GodotMeter : public godot::RefCounted {
		GDCLASS(GodotMeter, godot::RefCounted);

	protected:

		double value_{0.0};

		static void _bind_methods();

	public:

		GodotMeter();
		explicit GodotMeter(double value);

		[[nodiscard]] double get_value() const;

		static godot::Ref<GodotMeter> create(double value = 0.0);
};

class GodotLateral : public GodotMeter {
		GDCLASS(GodotLateral, GodotMeter);

	protected:

		static void _bind_methods();

	public:

		using GodotMeter::GodotMeter;

		static godot::Ref<GodotLateral> create(double value = 0.0);
};

class GodotHeight : public GodotMeter {
		GDCLASS(GodotHeight, GodotMeter);

	protected:

		static void _bind_methods();

	public:

		using GodotMeter::GodotMeter;

		static godot::Ref<GodotHeight> create(double value = 0.0);
};

class GodotDepth : public GodotMeter {
		GDCLASS(GodotDepth, GodotMeter);

	protected:

		static void _bind_methods();

	public:

		using GodotMeter::GodotMeter;

		static godot::Ref<GodotDepth> create(double value = 0.0);
};

class GodotVolume : public godot::RefCounted {
		GDCLASS(GodotVolume, godot::RefCounted);

	protected:

		double value_{0.0};

		static void _bind_methods();

	public:

		GodotVolume();
		explicit GodotVolume(double value);

		[[nodiscard]] double get_value() const;

		static godot::Ref<GodotVolume> create(double value = 0.0);
};

class GodotMass : public godot::RefCounted {
		GDCLASS(GodotMass, godot::RefCounted);

	protected:

		double value_{0.0};

		static void _bind_methods();

	public:

		GodotMass();
		explicit GodotMass(double value);

		[[nodiscard]] double get_value() const;

		static godot::Ref<GodotMass> create(double value = 0.0);
};

class GodotDensity : public godot::RefCounted {
		GDCLASS(GodotDensity, godot::RefCounted);

	protected:

		double value_{0.0};

		static void _bind_methods();

	public:

		GodotDensity();
		explicit GodotDensity(double value);

		[[nodiscard]] double get_value() const;

		static godot::Ref<GodotDensity> create(double value = 0.0);
};

class GodotSize : public godot::RefCounted {
		GDCLASS(GodotSize, godot::RefCounted);

	protected:

		godot::Ref<GodotLateral> lateral_;
		godot::Ref<GodotHeight>  height_;
		godot::Ref<GodotDepth>   depth_;

		static void _bind_methods();

	public:

		GodotSize();
		GodotSize(const godot::Ref<GodotLateral> &lateral,
				  const godot::Ref<GodotHeight>  &height,
				  const godot::Ref<GodotDepth>   &depth);

		[[nodiscard]] godot::Ref<GodotLateral> get_lateral() const;
		[[nodiscard]] godot::Ref<GodotHeight>  get_height() const;
		[[nodiscard]] godot::Ref<GodotDepth>   get_depth() const;
		[[nodiscard]] godot::Ref<GodotVolume>  get_volume() const;

		static godot::Ref<GodotSize> create(const godot::Ref<GodotLateral> &lateral = {},
											const godot::Ref<GodotHeight>  &height  = {},
											const godot::Ref<GodotDepth>   &depth   = {});

		static godot::Ref<GodotSize>
		from_values(double lateral = 0.0, double height = 0.0, double depth = 0.0);
};
