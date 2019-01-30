#include "../hook_manager.hh"
#include "../../cheat.hh"
#include "../../cfg.hh"
#include "../../features/misc/misc.hh"

int __fastcall hooks::post_screen_space_fx( void* ecx, void* edx, int a1 ) {
	features::misc::glow_bloom( );

	return client_mode->get_original<hooks::post_screen_space_fx_fn>( 44 )( interfaces::client_mode, a1 );
}
