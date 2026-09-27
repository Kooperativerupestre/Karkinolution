#pragma once

#include "godot/extension/model/math/stats/limited_value.hpp"

class GodotSharedVolume : public GodotLimitedValue {
		GDCLASS(GodotSharedVolume, GodotLimitedValue);

	protected:

		static void _bind_methods();

	public:

		GodotSharedVolume();
		explicit GodotSharedVolume(double value);

		static godot::Ref<GodotSharedVolume> create(double value = 0.0);
};

class GodotEfficiency : public GodotLimitedValue {
		GDCLASS(GodotEfficiency, GodotLimitedValue);

	protected:

		static void _bind_methods();

	public:

		GodotEfficiency();
		explicit GodotEfficiency(double value);

		static godot::Ref<GodotEfficiency> create(double value = 0.0);
};

class GodotQuality : public GodotLimitedValue {
		GDCLASS(GodotQuality, GodotLimitedValue);

	protected:

		static void _bind_methods();

	public:

		GodotQuality();
		explicit GodotQuality(double value);

		static godot::Ref<GodotQuality> create(double value = 0.0);
};
