#include "godot/extension/client/client.hpp"
#include "godot/extension/client/interpreters/creature.hpp"
#include "godot/extension/model/aliases.hpp"
#include "godot/extension/model/math/aliases.hpp"
#include "godot/extension/model/math/geometry.hpp"
#include "godot/extension/model/math/stats.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

void initialize_karkinolution(godot::ModuleInitializationLevel p_level) {
	if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	// Math - Limited Values
	GDREGISTER_CLASS(DoubleLimitedValue);
	GDREGISTER_CLASS(IntLimitedValue);

	// Math - Stats (inherit from DoubleLimitedValue)
	GDREGISTER_CLASS(GodotSharedVolume);
	GDREGISTER_CLASS(GodotEfficiency);
	GDREGISTER_CLASS(GodotQuality);

	// Math - Geometry
	GDREGISTER_CLASS(GodotRadius);
	GDREGISTER_CLASS(GodotCircumference);
	GDREGISTER_CLASS(GodotDiameter);
	GDREGISTER_CLASS(GodotArea);

	// Math - Units (base GodotMeter registered before derived classes)
	GDREGISTER_CLASS(GodotMeter);
	GDREGISTER_CLASS(GodotLateral);
	GDREGISTER_CLASS(GodotHeight);
	GDREGISTER_CLASS(GodotDepth);
	GDREGISTER_CLASS(GodotVolume);
	GDREGISTER_CLASS(GodotMass);
	GDREGISTER_CLASS(GodotDensity);
	GDREGISTER_CLASS(GodotSize);

	// Models
	GDREGISTER_CLASS(GodotProperty);
	GDREGISTER_CLASS(GodotDamage);
	GDREGISTER_CLASS(GodotMovementCost);
	GDREGISTER_CLASS(GodotSoil);
	GDREGISTER_CLASS(GodotCorpse);
	GDREGISTER_CLASS(GodotCreature);

	// Client & Interpreters
	GDREGISTER_CLASS(KarkinolutionGodot::Client::GodotClient);
	GDREGISTER_CLASS(KarkinolutionGodot::Interpreters::CreatureInterpreter);
}

void uninitialize_karkinolution(godot::ModuleInitializationLevel p_level) {
	if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
GDExtensionBool GDE_EXPORT
karkinolution_entry(GDExtensionInterfaceGetProcAddress p_get_proc_address,
					GDExtensionClassLibraryPtr         p_library,
					GDExtensionInitialization*         r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_karkinolution);
	init_obj.register_terminator(uninitialize_karkinolution);
	init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
	return init_obj.init();
}
}