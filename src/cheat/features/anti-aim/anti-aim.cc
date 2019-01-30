#include "anti-aim.hh"
#include "../../cfg.hh"
#include "../../cheat.hh"
#include "../../../utilities/interface_manager.hh"
#include <algorithm>

bool features::anti_aim::break_lowerbody;
bool features::anti_aim::first_break;
float features::anti_aim::next_lowerbody_update;

#define interval			( interfaces::global_vars->interval_per_tick )
#define time_to_ticks( dt )		( (int)( 0.5f + (float)(dt) / interval ) )
#define ticks_to_time( t )		( interval *( t ) )


#define degrees_to_radians( x ) ( ( float )( x ) * ( float )( ( float )( 3.14159265358979323846f ) / 180.0f ) )

void features::anti_aim::execute( sdk::user_cmd* command ) {
	if ( !g_config.anti_aim.enable )
		return;

	command->view_angles.x = pitch( command );

	command->view_angles.y += !csgo::send_packet ? fake_angles( command ) : real_angles( command );

	auto normalize_float = []( float yaw ) -> float {
		if ( yaw > 180 ) {
			yaw -= ( round( yaw / 360 ) * 360.f );
		}
		else if ( yaw < -180 ) {
			yaw += ( round( yaw / 360 ) * -360.f );
		}
		return yaw;
	};


	if ( g_config.anti_aim.fake_body ) {
		if ( break_lowerbody ) {
			command->view_angles.y += 98.f;
			csgo::lowerbody_angles = command->view_angles;
		}
	}
}

void features::anti_aim::update_lowerbody_breaker( sdk::user_cmd* command ) {
	if ( !csgo::local_player->alive( ) )
		return;

	float server_time = corrected_time( ), speed = csgo::unpredicted_velocity.length_2d();

	if ( speed > 0.1 ) {
		next_lowerbody_update = server_time + 0.22;
		first_break = true;
	}

	break_lowerbody = false;

	if ( next_lowerbody_update < server_time ) {
		next_lowerbody_update = server_time + 1.1;
		break_lowerbody = true;
		first_break = false;
	}

	if ( !( csgo::local_player->flags( ) & ( 1 << 0) ) ) {
		break_lowerbody = false;
	}
}

float features::anti_aim::pitch( sdk::user_cmd* command ) {
	float pitch = command->view_angles.x;

	switch ( g_config.anti_aim.pitch ) {
	case 0:
		break;
	case 1:
		pitch = 89.0f;
		break;
	case 2:
		pitch = -89.0f;
		break;
	}
	return pitch;
}

float features::anti_aim::real_angles( sdk::user_cmd* command ) {
	int index;

	if ( csgo::local_player->velocity( ).length( ) > 1.f && csgo::local_player->flags( ) & ( 1 << 0 ) ) {
		index = g_config.anti_aim.move.yaw;
	}
	else if ( csgo::local_player->flags( ) & ( 1 << 0 ) && csgo::local_player->velocity( ).length( ) < 1.f ) {
		index = g_config.anti_aim.stand.yaw;
	}
	else if ( !( csgo::local_player->flags( ) & ( 1 << 0 ) ) ) {
		index = g_config.anti_aim.air.yaw;
	}

	float yaw = command->view_angles.y;

	if ( index ) {
		yaw = 0.0f;

		sdk::vec3 angles = interfaces::engine_client->get_view_angles( );
		yaw = angles.y;

		switch ( index ) {
		case 0:
			break;
		case 1:
			yaw = 180;
			break;
		case 2:
			yaw = interfaces::global_vars->cur_time * 425.0f;
			break;
		case 3:
			yaw = 90.f;
			break;
		case 4:
			if ( index == g_config.anti_aim.air.yaw ) {
				constexpr auto max_range = 90.0f;
				constexpr auto additive = 180.0f;

				if ( csgo::local_player->flags( ) & ( 1 << 0 ) ) {
					yaw = max_range;
				}
				else {
					yaw = additive - max_range / 2.f + std::fmodf( interfaces::global_vars->cur_time * 150, max_range );
				}
			}
			else {
				constexpr auto max_range = 90.0f;
				constexpr auto additive = 180.0f;
				yaw = additive - max_range / 2.f + std::fmodf( interfaces::global_vars->cur_time * 150, max_range );
			}
			break;
		}
	}

	return yaw;
}

float features::anti_aim::fake_angles( sdk::user_cmd* command ) {
	int index, offset;

	index = g_config.anti_aim.fake_yaw;

	float yaw = command->view_angles.y;

	if ( index ) {
		yaw = 0.0f;

		switch ( index ) {
		case 0:
			break;
		case 1:
			yaw = 180.f;
			break;
		case 2:
			yaw = interfaces::global_vars->cur_time * 425.0f;
			break;
		case 3:
			yaw = 90.f;
			break;
		case 4:
			yaw = csgo::real_angles.y + 180.f;
			break;
		}
	}
	return yaw;
}

void features::anti_aim::correct_movement( sdk::user_cmd * command, sdk::vec3 old_angles, float old_forward, float old_sidemove ) { /* keep this last because its really messy */
	if ( command->forward_move >= 450 ) command->forward_move = 450;
	if ( command->side_move >= 450 ) command->side_move = 450;
	if ( command->up_move >= 450 ) command->up_move = 450;
	if ( command->forward_move <= -450 ) command->forward_move = -450;
	if ( command->side_move <= -450 ) command->side_move = -450;
	if ( command->up_move <= -450 ) command->up_move = -450;

	// side/forward move correction
	float delta;
	float f1;
	float f2;

	if ( old_angles.y < 0.f )
		f1 = 360.0f + old_angles.y;
	else
		f1 = old_angles.y;

	if ( command->view_angles.y < 0.0f )
		f2 = 360.0f + command->view_angles.y;
	else
		f2 = command->view_angles.y;

	if ( f2 < f1 )
		delta = abs( f2 - f1 );
	else
		delta = 360.0f - abs( f1 - f2 );

	delta = 360.0f - delta;

	command->forward_move = cos( degrees_to_radians( delta ) ) * old_forward + cos( degrees_to_radians( delta + 90.f ) ) * old_sidemove;
	command->side_move = sin( degrees_to_radians( delta ) ) * old_forward + sin( degrees_to_radians( delta + 90.f ) ) * old_sidemove;

}

float features::anti_aim::corrected_time( sdk::user_cmd* command ) {
	if ( csgo::local_player ) {
		static int current_tick = 0;
		static sdk::user_cmd* last_command;

		if ( !command )
			return current_tick * interfaces::global_vars->interval_per_tick;

		if ( !last_command || last_command->has_been_predicted )
			current_tick = csgo::local_player->tick_base( );
		else
			current_tick++;

		last_command = command;
	}
}
