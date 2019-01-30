#pragma once
#include "../../../sdk/classes/user_cmd.hh"
#include "../../../sdk/interfaces/game_prediction.hh"

namespace features::engine_prediction {
	extern float old_cur_time;
	extern float old_frame_time;
	extern int* prediction_random_seed;
	extern sdk::player_move_data move_data;

	void start_prediction( sdk::user_cmd* command );
	void end_prediction( );
}
