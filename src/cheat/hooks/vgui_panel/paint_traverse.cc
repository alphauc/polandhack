#include "../hook_manager.hh"
#include "../../../utilities/renderer.hh"
#include "../../features/visuals/esp.hh"
#include "../../features/anti-aim/anti-aim.hh"
#include "../../features/misc/misc.hh"
#include "../../features/misc/dormancy.hh"
#include "../../cfg.hh"

void __stdcall hooks::paint_traverse( sdk::vgui::panel panel, bool force_repaint, bool allow_force ) {
	static sdk::vgui::panel hud_zoom_panel = 0;

	if ( !hud_zoom_panel ) {
		if ( std::string( interfaces::vgui_panel->get_name( panel ) ).find( "HudZoom" ) != std::string::npos )
			hud_zoom_panel = panel;
	}

	if ( interfaces::engine_client->is_connected( ) && interfaces::engine_client->is_in_game( ) ) {
		if ( g_config.visuals.remove_scope && hud_zoom_panel == panel )
			return;
	}

	vgui_panel->get_original<hooks::paint_traverse_fn>( 41 )(
		interfaces::vgui_panel, panel,
		force_repaint, allow_force
		);

	static sdk::vgui::panel focus_overlay_panel = 0;

	if ( !focus_overlay_panel ) {
		if ( std::string( interfaces::vgui_panel->get_name( panel ) ).find( "FocusOverlayPanel" ) != std::string::npos ) {
			focus_overlay_panel = panel;

			render::fonts::visuals_general = interfaces::vgui_surface->allocate_font( );
			render::fonts::visuals_flags = interfaces::vgui_surface->allocate_font( );
			render::fonts::misc_specs = interfaces::vgui_surface->allocate_font( );

			interfaces::vgui_surface->set_font( render::fonts::visuals_general, "Verdana", 12, FW_BOLD, sdk::vgui::font_flags::drop_shadow );
			interfaces::vgui_surface->set_font( render::fonts::visuals_flags, "Small Fonts", 9, FW_NORMAL, sdk::vgui::font_flags::outline );
			interfaces::vgui_surface->set_font( render::fonts::misc_specs, "Tahoma", 12, FW_NORMAL, sdk::vgui::font_flags::drop_shadow );
		}
	}

	if ( panel != focus_overlay_panel )
		return;

	features::misc::offscreen_esp( );
	features::misc::spectators( );
	features::misc::scope_lines( );

	//-------------------------------------------------
	features::esp::execute( );
	//-------------------------------------------------
}
