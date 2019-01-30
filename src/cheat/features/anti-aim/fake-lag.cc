#include "fake-lag.hh"
#include "../../cheat.hh"
#include "../../../utilities/interface_manager.hh"
#include "../../../sdk/misc/vector.hh"
#include "../../cfg.hh"

void features::fake_lag::execute() {
	static int ticks = 0;
	int choke;
	bool should_fakelag = false;

	switch (g_config.anti_aim.fakelag_type) {
		case 0:
			choke = g_config.anti_aim.fakelag_packets;
			break;
		case 1:
			choke = std::min<int>(static_cast<int>(std::ceilf(64 / (csgo::local_player->velocity().length() * interfaces::global_vars->interval_per_tick))), g_config.anti_aim.fakelag_packets);
			break;
		case 2:
			choke = lag_comp_break();
			break;
	}


	if (g_config.anti_aim.fakelag_check_standing) {
		if (csgo::local_player->velocity().length() < 0.1f && csgo::local_player->flags() & FL_ONGROUND)
			should_fakelag = true;
	}

	if (g_config.anti_aim.fakelag_check_moving) {
		if (csgo::local_player->velocity().length() > 0.1f && csgo::local_player->flags() & FL_ONGROUND)
			should_fakelag = true;
	}

	if (g_config.anti_aim.fakelag_check_air) {
		if (!(csgo::local_player->flags() & FL_ONGROUND))
			should_fakelag = true;
	}

	if (should_fakelag) {
		if (ticks > choke) {
			ticks = 0;
			csgo::send_packet = true;
		}
		else {
			csgo::send_packet = false;
			ticks++;
		}
		csgo::fakelagging = true;
	}
	else
		csgo::fakelagging = false;
}

int features::fake_lag::lag_comp_break() {
	sdk::vec3 velocity = csgo::local_player->velocity(); velocity.z = 0;
	float speed = velocity.length();

	if (speed > 0.f) {
		auto distance_per_tick = speed * interfaces::global_vars->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, g_config.anti_aim.fakelag_packets);
	}

	return 1;
}