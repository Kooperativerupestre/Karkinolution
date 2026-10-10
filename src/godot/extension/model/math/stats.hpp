#pragma once

#include "godot/extension/model/math/limited_values.hpp"

using KarkinolutionGodot::Models::Math::Stats::DoubleLimitedValue;

class GodotSharedVolume : public DoubleLimitedValue {
		GDCLASS(GodotSharedVolume, DoubleLimitedValue);

	protected:

		static void _bind_methods() {}

	public:
};

class GodotEfficiency : public DoubleLimitedValue {
		GDCLASS(GodotEfficiency, DoubleLimitedValue);

	protected:

		static void _bind_methods() {}


	public:
};

class GodotQuality : public DoubleLimitedValue {
		GDCLASS(GodotQuality, DoubleLimitedValue);

	protected:

		static void _bind_methods() {}

	public:
};