#pragma once

namespace sdk {

	class global_vars {
	public:
		float     real_time;
		int       frame_count;
		float     absolute_frametime;
		float     absolute_framestarttimestddev;
		float     cur_time;
		float     frame_time;
		int       max_clients;
		int       tick_count;
		float     interval_per_tick;
		float     interpolation_amount;
		int       sim_ticks_this_frame;
		int       network_protocol;
		void*     p_save_data;
		bool      b_client;
		bool      b_remote_client;
	};

}