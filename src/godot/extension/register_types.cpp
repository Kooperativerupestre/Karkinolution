#include "binary/frame.hpp"
#include "karkinolution.hpp"
#include "model/creature/creature.hpp"
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
	GDREGISTER_CLASS(GodotEntityStorage);
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