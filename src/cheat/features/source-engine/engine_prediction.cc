#include "engine_prediction.hh"
#include "../../../utilities/interface_manager.hh"
#include "../../../utilities/input.hh"
#include "../../../utilities/renderer.hh"
#include "../../../utilities/utilities.hh"
#include "../../../utilities/math.hh"
#include "../../../utilities/md5.hh"
#include "../../cheat.hh"
#include "../../cfg.hh"
#include <iostream>
#include <fstream>

float features::engine_prediction::old_cur_time;
float features::engine_prediction::old_frame_time;
int* features::engine_prediction::prediction_random_seed;
sdk::player_move_data features::engine_prediction::move_data;

void features::engine_prediction::start_prediction( sdk::user_cmd* command ) {
	if ( csgo::local_player ) {
		static bool initialized = false;
		if ( !initialized ) {
			prediction_random_seed = *( int** )( utilities::find_occurrence( "client_panorama.dll", "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04" ) + 2 );
			initialized = true;
		}

		*prediction_random_seed = utilities::md5::pseduo_random( command->cmd_number ) & 0x7FFFFFFF;

		old_cur_time = interfaces::global_vars->cur_time;
		old_frame_time = interfaces::global_vars->frame_time;

		interfaces::global_vars->cur_time = csgo::local_player->tick_base( ) * interfaces::global_vars->interval_per_tick;
		interfaces::global_vars->frame_time = interfaces::global_vars->interval_per_tick;

		interfaces::player_game_movement->start_track_prediction_errors( csgo::local_player );

		memset( &move_data, 0, sizeof( move_data ) );
		interfaces::player_move_helper->set_host( csgo::local_player );
		interfaces::player_prediction->setup_move( csgo::local_player, command, interfaces::player_move_helper, &move_data );
		interfaces::player_game_movement->process_movement( csgo::local_player, &move_data );
		interfaces::player_prediction->finish_move( csgo::local_player, command, &move_data );

	}
}

void features::engine_prediction::end_prediction( ) {
	if ( csgo::local_player ) {
		interfaces::player_game_movement->finish_track_prediction_errors( csgo::local_player );
		interfaces::player_move_helper->set_host( 0 );

		*prediction_random_seed = -1;

		interfaces::global_vars->cur_time = old_cur_time;
		interfaces::global_vars->frame_time = old_cur_time;
	}
}