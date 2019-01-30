#include "../hook_manager.hh"
#include "../../cfg.hh"
#include "../../cheat.hh"
#include "../../../utilities/renderer.hh"
#include "../../gui/menu.hh"

long __stdcall hooks::reset(sdk::d3d_device_9 *device, sdk::d3d_present_parameters *present_parameters) {

	if (csgo::initialized) {

		console::log("re-initializing...");
		object::helper::destroy(); // release sprites

		render::invalidate(); // invalidate our renderer
		HRESULT hr = d3d_device->get_original<hooks::reset_fn>(16)(device, present_parameters); // save the return value
		render::restore(device);  // re-initialize our renderer

		object::helper::setup(device, 150, 150); // re-initialize sprites
		console::log("re-initializing ended in success.");
		return hr;
	}

	return d3d_device->get_original<hooks::reset_fn>(16)(device, present_parameters);
}