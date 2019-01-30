#include "../hook_manager.hh"
#include "../../cheat.hh"
#include "../../cfg.hh"
#include "../../features/anti-aim/anti-aim.hh"
#include "../../features/source-engine/engine_prediction.hh"
#include "../../features/misc/misc.hh"
#include "../../features/anti-aim/fake-lag.hh"
#include "../../features/aimbot/aimbot.hh"

bool __stdcall hooks::create_move( float frame_time, sdk::user_cmd* user_cmd ) {
	auto original_fn = client_mode->get_original<hooks::create_move_fn>( 24 );

	if ( !user_cmd || user_cmd->cmd_number == 0 )
		return original_fn( interfaces::client_mode, frame_time, user_cmd );

	features::anti_aim::corrected_time( user_cmd );

	uintptr_t *frame_ptr;
	__asm mov frame_ptr, ebp;

	csgo::send_packet = true;


	sdk::vec3 old_angle = user_cmd->view_angles;
	float old_forward = user_cmd->forward_move;
	float old_side = user_cmd->side_move;

	auto local = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
	if ( local ) {
		// incase local is nullptr, which will never happen unless we are not in game, but eh, lets be on the safe side. 
		csgo::local_player = local;

		if ( g_config.movement.bhop ) {
			if ( !( csgo::local_player->flags( ) & ( 1 << 0 ) ) && user_cmd->buttons & ( 1 << 1 ) )
				user_cmd->buttons &= ~( 1 << 1 );
		}

		//features::fake_lag::execute(); //swapping from real to fake will look into it xd

		csgo::unpredicted_velocity = local->velocity( );
		features::engine_prediction::start_prediction( user_cmd ); {
			features::anti_aim::update_lowerbody_breaker( user_cmd );
			features::anti_aim::execute( user_cmd );
			features::anti_aim::correct_movement( user_cmd, old_angle, old_forward, old_side );
		//	features::ragebot::aimbot.execute(user_cmd);

		}
		features::engine_prediction::end_prediction( );

		//features::ragebot::correction.execute_create_move();
	}

	if ( interfaces::client_state->choked_commands > 14 )
		csgo::send_packet = true;

	*( bool* )( *frame_ptr - 0x1C ) = csgo::send_packet;

	static sdk::vec3 last_choked_angles;

	if ( !csgo::send_packet && interfaces::client_state->choked_commands == 0 )
		last_choked_angles = user_cmd->view_angles;
	else if ( csgo::send_packet && interfaces::client_state->choked_commands > 0 )
		csgo::real_angles = last_choked_angles;
	else if ( csgo::send_packet && interfaces::client_state->choked_commands == 0 )
		csgo::real_angles = user_cmd->view_angles;

	if ( csgo::send_packet )
		csgo::fake_angles = user_cmd->view_angles;

	return false;
}
