#include "../../utilities/renderer.hh"
#include "../features/visuals/esp.hh"
#include "../features/anti-aim/anti-aim.hh"
#include "../features/misc/misc.hh"
#include "../gui/menu.hh"
#include "../cheat.hh"
#include "hook_manager.hh"
#include <fstream>

std::unique_ptr<security::vmt_hook> hooks::client_dll = nullptr;
std::unique_ptr<security::vmt_hook> hooks::d3d_device = nullptr;
std::unique_ptr<security::vmt_hook> hooks::vgui_surface = nullptr;
std::unique_ptr<security::vmt_hook> hooks::client_mode = nullptr;
std::unique_ptr<security::vmt_hook> hooks::vgui_panel = nullptr;
std::unique_ptr<security::vmt_hook> hooks::render_view = nullptr;

void hooks::initialize( ) {
	client_dll = std::make_unique<security::vmt_hook>( interfaces::client_dll );
	client_dll->swap_fn<37>( hooks::frame_stage_notify );

	d3d_device = std::make_unique<security::vmt_hook>( interfaces::d3d_device );
	d3d_device->swap_fn<16>( hooks::reset );
	d3d_device->swap_fn<42>( hooks::end_scene );

	vgui_surface = std::make_unique<security::vmt_hook>( interfaces::vgui_surface );
	vgui_surface->swap_fn<67>( hooks::lock_cursor );

	client_mode = std::make_unique<security::vmt_hook>( interfaces::client_mode );
	client_mode->swap_fn<24>( hooks::create_move );
	client_mode->swap_fn<44>( hooks::post_screen_space_fx );
	client_mode->swap_fn<18>(hooks::override_view);

	vgui_panel = std::make_unique <security::vmt_hook>( interfaces::vgui_panel );
	vgui_panel->swap_fn<41>( hooks::paint_traverse );

	render_view = std::make_unique <security::vmt_hook>( interfaces::render_view );
	render_view->swap_fn<9>( hooks::scene_end );
}

void hooks::shutdown( ) {
	client_dll->~vmt_hook( );
	d3d_device->~vmt_hook( );
	vgui_surface->~vmt_hook( );
	client_mode->~vmt_hook( );
	vgui_panel->~vmt_hook( );
	render_view->~vmt_hook( );
}
