#pragma once

#include "../../utilities/interface_manager.hh"
#include "../../utilities/console.hh"
#include "../../sdk/misc/definitions.hh"
#include "../../sdk/classes/user_cmd.hh"
#include "../../sdk/classes/view_setup.hh"
#include "../../security/vmt.hh"
#include "../../utilities/utilities.hh"

namespace hooks {

	extern std::unique_ptr<security::vmt_hook> client_dll;
	extern std::unique_ptr<security::vmt_hook> d3d_device;
	extern std::unique_ptr<security::vmt_hook> vgui_surface;
	extern std::unique_ptr<security::vmt_hook> client_mode;
	extern std::unique_ptr<security::vmt_hook> vgui_panel;
	extern std::unique_ptr<security::vmt_hook> render_view;

	void initialize( );
	void shutdown( );

	using frame_stage_notify_fn = void( __thiscall * )( void *, int );
	using end_scene_fn = long( __stdcall * )( sdk::d3d_device_9 * );
	using reset_fn = long( __stdcall * )( sdk::d3d_device_9 *, sdk::d3d_present_parameters * );
	using lock_cursor_fn = void( __thiscall* )( void* );
	using create_move_fn = bool( __thiscall* )( sdk::client_mode*, float, sdk::user_cmd * );
	using paint_traverse_fn = void( __thiscall * )( void *, sdk::vgui::panel, bool, bool );
	using scene_end_fn = void( __thiscall * )( void * );
	using post_screen_space_fx_fn = bool( __thiscall * )( void*, int );
	using write_cmd_fn = bool( __thiscall* )( void*, int, void *, int, int, bool );
	using override_view_fn = void(__stdcall*)(sdk::view_setup*);

	void __stdcall frame_stage_notify( int stage );
	long __stdcall end_scene( sdk::d3d_device_9 *device );
	long __stdcall reset( sdk::d3d_device_9 *device, sdk::d3d_present_parameters *present_parameters );
	void __stdcall lock_cursor( );
	bool __stdcall create_move( float frame_time, sdk::user_cmd * user_cmd );
	void __stdcall paint_traverse( sdk::vgui::panel panel, bool force_repaint, bool allow_force );
	void __stdcall scene_end( );
	int __fastcall post_screen_space_fx( void * ecx, void * edx, int a1 );
	bool __fastcall write_cmd( void *ecx, void *edx, int slot, void *buf, int from, int to, bool new_cmd );
	void __stdcall override_view(sdk::view_setup* viewsetup);

}