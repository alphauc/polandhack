#pragma once
#include "../../../sdk/misc/vector.hh"
#include "../../../utilities/math.hh"
#include "../../../sdk/classes/user_cmd.hh"

namespace features::anti_aim {
	extern bool break_lowerbody;
	extern bool first_break;
	extern float next_lowerbody_update;

	void execute( sdk::user_cmd *command );
	void update_lowerbody_breaker( sdk::user_cmd * command );
	float pitch( sdk::user_cmd *command );
	float real_angles( sdk::user_cmd *command );
	float fake_angles( sdk::user_cmd *command );
	void correct_movement( sdk::user_cmd * command, sdk::vec3 old_angles, float old_forward, float old_sidemove );
	float corrected_time( sdk::user_cmd * command = nullptr);
}