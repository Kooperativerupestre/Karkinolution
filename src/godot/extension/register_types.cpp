#include "binary/frame.hpp"
#include "karkinolution.hpp"
#include "model/creature/creature.hpp"
#include "model/math/geometry/geometry.hpp"
#include "model/math/stats/limited_value.hpp"
#include "model/math/stats/stats.hpp"
#include "model/math/unit/units.hpp"
#include "model/properties/properties.hpp"
#include "model/terrain/soil.hpp"
#include "storage/entity_storage.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_karkinolution(ModuleInitializationLevel level) {
	if (level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(GodotParsedFrame);
	GDREGISTER_CLASS(GodotCreature);
	GDREGISTER_CLASS(GodotGenericProperty);
	GDREGISTER_CLASS(GodotSoilPiece);
	GDREGISTER_CLASS(GodotEntityStorage);

	GDREGISTER_CLASS(GodotLimitedValue);
	GDREGISTER_CLASS(GodotSharedVolume);
	GDREGISTER_CLASS(GodotEfficiency);
	GDREGISTER_CLASS(GodotQuality);

	GDREGISTER_CLASS(GodotMeter);
	GDREGISTER_CLASS(GodotLateral);
	GDREGISTER_CLASS(GodotHeight);
	GDREGISTER_CLASS(GodotDepth);
	GDREGISTER_CLASS(GodotVolume);
	GDREGISTER_CLASS(GodotMass);
	GDREGISTER_CLASS(GodotDensity);
	GDREGISTER_CLASS(GodotSize);

	GDREGISTER_CLASS(GodotRadius);
	GDREGISTER_CLASS(GodotCircumference);
	GDREGISTER_CLASS(GodotDiameter);
	GDREGISTER_CLASS(GodotArea);

	GDREGISTER_CLASS(Karkinolution);
}

void uninitialize_karkinolution(ModuleInitializationLevel level) {
	if (level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {

GDExtensionBool karkinolution_extension_init(GDExtensionInterfaceGetProcAddress get_proc_address,
											 GDExtensionClassLibraryPtr         library,
											 GDExtensionInitialization*         initialization) {
	GDExtensionBinding::InitObject init_obj(get_proc_address, library, initialization);

	init_obj.register_initializer(initialize_karkinolution);
	init_obj.register_terminator(uninitialize_karkinolution);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}