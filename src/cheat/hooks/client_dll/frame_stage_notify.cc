#include "../hook_manager.hh"
#include "../../cheat.hh"
#include "../../features/misc/misc.hh"
#include "../../features/aimbot/backtracking.hh"
#include "../../features/misc/dormancy.hh"

void __stdcall hooks::frame_stage_notify( int stage ) {
	if ( stage == sdk::FRAME_RENDER_START) {
		if ( csgo::local_player ) {
			if ( csgo::local_player->alive( ) ) {
				if ( *( bool* )( ( uintptr_t )interfaces::input + 0xA5 ) )
					*( sdk::vec3* )( ( uintptr_t )csgo::local_player + 0x31C8 ) = csgo::real_angles;
			}

		}

		//features::misc::thirdperson();
		features::misc::remove_flash();
		features::misc::remove_smoke();
		features::misc::world_modulation();
	}
	if (stage == sdk::FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		features::misc::dormancy::position_correction( stage );
	//	features::ragebot::correction.execute_frame_stage_notify();
	}

	if (stage == sdk::FRAME_NET_UPDATE_POSTDATAUPDATE_END) {
	//	features::backtracking::backtrack.store();
	}

	client_dll->get_original<hooks::frame_stage_notify_fn>( 37 )( interfaces::client_dll, stage );
}
