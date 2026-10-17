#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace KarkinolutionGodot {
	namespace Models {
		namespace Math {
			namespace Units {
				class GodotMeter : public godot::RefCounted {
						GDCLASS(GodotMeter, godot::RefCounted);

					protected:

						double value_{0.0};

						static void _bind_methods();

					public:

						GodotMeter();
						explicit GodotMeter(double value);

						[[nodiscard]] double get_value() const;

						void set_value(double value) {
							value_ = value;
						}
				};

				class GodotLateral : public GodotMeter {
						GDCLASS(GodotLateral, GodotMeter);

					protected:

						static void _bind_methods();

					public:

						using GodotMeter::GodotMeter;
				};

				class GodotHeight : public GodotMeter {
						GDCLASS(GodotHeight, GodotMeter);

					protected:

						static void _bind_methods();

					public:

						using GodotMeter::GodotMeter;
				};

				class GodotDepth : public GodotMeter {
						GDCLASS(GodotDepth, GodotMeter);

					protected:

						static void _bind_methods();

					public:

						using GodotMeter::GodotMeter;
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

						void set_value(double value) {
							value_ = value;
						}
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

						void set_value(double value) {
							value_ = value;
						}
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

						void set_value(double value) {
							value_ = value;
						}
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
						GodotSize(double lateral, double height, double depth);

						[[nodiscard]] double get_lateral() const;
						[[nodiscard]] double get_height() const;
						[[nodiscard]] double get_depth() const;
						[[nodiscard]] double get_volume() const;

						void set_value(double lateral, double height, double depth) {
							if (!lateral_.is_valid()) {
								lateral_.instantiate();
							}
							lateral_->set_value(lateral);

							if (!height_.is_valid()) {
								height_.instantiate();
							}
							height_->set_value(height);

							if (!depth_.is_valid()) {
								depth_.instantiate();
							}
							depth_->set_value(depth);
						}
				};
			} // namespace Units
		} // namespace Math
	} // namespace Models
} // namespace KarkinolutionGodot
