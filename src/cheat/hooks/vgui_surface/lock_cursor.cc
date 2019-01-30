#include "../hook_manager.hh"
#include "../../cfg.hh"

void __stdcall hooks::lock_cursor() {
	if (g_config.menu.m_open)
		interfaces::vgui_surface->unlock_cursor();
	else
		vgui_surface->get_original<hooks::lock_cursor_fn>(67)(interfaces::vgui_surface);
}
