#pragma once

#include "../../cheat.hh"
#include "../../../utilities/interface_manager.hh"
#include "../../../utilities/math.hh"
#include "resolver/resolver.hh"

namespace sdk {
#define time_to_ticks( dt )		( (int)( 0.5 + (float)(dt) / interfaces::global_vars->interval_per_tick ) )
#define ticks_to_time( t )		( interfaces::global_vars->interval_per_tick *( t ) )
}

namespace features::backtracking {

#define enable_interpolation 1
#define disable_interpolation 0

	static constexpr float MAX_UNLAG = 0.2f;
	static constexpr float MAX_BACKTRACK_RANGE = 0.2f;

	struct backtracking_record {
		backtracking_record( ) {
		};
		explicit backtracking_record( sdk::base_entity* );

		sdk::vec3 vec_origin;
		sdk::vec3 eye_angles;
		sdk::vec3 abs_angles;
		sdk::vec3 render_angles;
		sdk::vec3 bbmin;
		sdk::vec3 bbmax;
		sdk::vec3 velocity;
		sdk::vec3 previous_vec_origin;

		float simulation_time;
		float previous_simulation_time;
		float curtime;
		float previous_curtime;
		bool is_exploiting;
		int flags;

		sdk::base_entity* player;
		sdk::animation_layer anim_layers[ 15 ];
		sdk::base_anim_state animstate;
		sdk::matrix3x4_t bone_matrix[ 128 ];
		float pose_params[ 24 ];

		features::ragebot::resolver::player_resolve_record resolve_record;
	};

	class backtracking {
	public:
		void store( );
		void restore( sdk::base_entity*, backtracking_record, int extrapolate_ticks = 0 );
		void apply_restore( sdk::base_entity* entity, float curtime );
		void restore_to_current_record( sdk::base_entity* );

		std::vector<backtracking_record> priority_record( sdk::base_entity* entity ); // gets the records in order of most usable, lby update, etcs
		std::vector<backtracking_record> valid_records( sdk::base_entity* entity );
		std::vector<backtracking_record> get_records( sdk::base_entity* entity );
		backtracking_record* get_current_record( sdk::base_entity* entity );

		std::vector<backtracking_record>& get_shot_at_records( ) {
			return shot_at_records;
		}

		void add_shot_record( backtracking_record record ) {
			shot_at_records.insert( shot_at_records.begin( ), record );

			if ( shot_at_records.size( ) > 3 )
				shot_at_records.pop_back( );
		}

		bool get_extrapolated_record( sdk::base_entity* entity, backtracking_record& record );
		bool extrapolation( sdk::base_entity* entity, backtracking_record& record, float time );

		static float get_delta_time( backtracking_record record );
		int fix_tick_count( sdk::base_entity * player, backtracking_record & record );
		bool is_tick_valid( int tick );
		static int get_lerp_ticks( );
		static int get_tick_count( backtracking_record );
		static void set_interpolation_flags( sdk::base_entity* entity, int flag );
		backtracking_record recordss;

	private:
		std::vector<backtracking_record> shot_at_records;
		bool needs_to_restore[ 64 ];
		int last_incoming_sequence = 0;
		//std::deque<Incoming_Sequence_Record> sequence_records;
		std::vector<backtracking_record> player_records[ 64 ];

		static void invalidate_bone_cache( sdk::base_entity* entity );
	};

	extern backtracking backtrack;
}