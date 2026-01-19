#include "register_types.h"

#include "video_player.h"
#include "video_player_soft.h"

#if defined(__APPLE__)
#include "video_player_metal.h"
#endif

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_vtg_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)  {
        return;
    }

    GDREGISTER_ABSTRACT_CLASS(VideoPlayer);
    GDREGISTER_CLASS(VideoPlayerSoft);
    
#if defined(__APPLE__)   
    GDREGISTER_CLASS(VideoPlayerMetal);
#elif defined(__linux__)
#elif defined(_WIN32)
#endif

}

void uninitialize_vtg_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C"  {
GDExtensionBool GDE_EXPORT vtg_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, \
                                                const GDExtensionClassLibraryPtr p_library, \
                                                GDExtensionInitialization *r_initialization) 
{
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_vtg_module);
	init_obj.register_terminator(uninitialize_vtg_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
    
	return init_obj.init();
}
}
