#include "../hook_manager.hh"
#include "../../cfg.hh"
#include "../../cheat.hh"
#include "../../../utilities/renderer.hh"
#include "../../gui/menu.hh"

long __stdcall hooks::end_scene(sdk::d3d_device_9 *device) {
	if (!csgo::initialized) {
		render::initialize();
		csgo::initialized = true;
	}

	if (csgo::initialized) {
		render::begin();

		menu::ui.on_frame();

		render::end();
	}

	return d3d_device->get_original<hooks::end_scene_fn>(42)(device);
}
