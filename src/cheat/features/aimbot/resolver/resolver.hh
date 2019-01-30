#pragma once

#include "../../../cheat.hh"
#include "../../../../utilities/interface_manager.hh"
#include "../../../../utilities/math.hh"
#include "../../../../sdk/interfaces/game_events.hh"

namespace features::ragebot{
	class resolver
	{
	public:
		resolver( );

		void execute_frame_stage_notify( );
		void execute_create_move( );

	public:
		/// resolve types, they're "flags" so to speak since a player can have more than 1 resolve type at once
		/// if angles overlap
		static const unsigned int RESOLVE_TYPE_NUM = 8;
		static const unsigned short RESOLVE_TYPE_OVERRIDE = 0b00000001,
			RESOLVE_TYPE_NO_FAKE = 0b00000010,
			RESOLVE_TYPE_LBY = 0b00000100,
			RESOLVE_TYPE_LBY_UPDATE = 0b00001000,
			RESOLVE_TYPE_PREDICTED_LBY_UPDATE = 0b00010000,
			RESOLVE_TYPE_LAST_MOVING_LBY = 0b00100000,
			RESOLVE_TYPE_NOT_BREAKING_LBY = 0b01000000,
			RESOLVE_TYPE_BRUTEFORCE = 0b10000000,
			RESOLVE_TYPE_LAST_MOVING_LBY_DELTA = 0b100000000,
			RESOLVE_TYPE_ANTI_FREESTANDING = 0b1000000000;

	public:
		/// a struct holding info the resolver needs, updated every frame for every player
		class player_resolve_record
		{
		public:
			player_resolve_record( )
			{
				resolve_type = 0;
				shots_missed_moving_lby = 0;
				shots_missed_moving_lby_delta = 0;

				last_balance_adjust_trigger_time = 0.f;
				last_moving_lby_delta = 0.f;
				last_time_moving = 0.f;
				last_time_down_pitch = 0.f;
				next_predicted_lby_update = 0.f;

				has_fake = false;
				is_dormant = false, is_last_moving_lby_delta_valid = false;
				is_last_moving_lby_valid = false, is_fakewalking = false;
				is_balance_adjust_triggered = false, is_balance_adjust_playing = false;
				did_lby_flick = false, did_predicted_lby_flick = false;

				for ( int i = 0; i < RESOLVE_TYPE_NUM; i++ )
				{
					shots_hit[ i ] = 0;
					shots_fired[ i ] = 0;
				}
			}

		public:
			struct AntiFreestandingRecord
			{
				int right_damage = 0, left_damage = 0;
				float right_fraction = 0.f, left_fraction = 0.f;
			};

		public:
			sdk::animation_layer anim_layers[ 15 ];
			AntiFreestandingRecord anti_freestanding_record;

			sdk::vec3 resolved_angles, networked_angles;
			sdk::vec3 velocity, origin;

			int shots_hit[ RESOLVE_TYPE_NUM ], shots_fired[ RESOLVE_TYPE_NUM ];
			int shots_missed_moving_lby, shots_missed_moving_lby_delta;
			unsigned short resolve_type;

			float lower_body_yaw;
			float last_moving_lby;
			float last_moving_lby_delta;
			float last_balance_adjust_trigger_time;
			float last_time_moving;
			float last_time_down_pitch;
			float next_predicted_lby_update;

			bool is_dormant;
			bool is_last_moving_lby_valid;
			bool is_fakewalking;
			bool is_last_moving_lby_delta_valid;
			bool is_balance_adjust_triggered, is_balance_adjust_playing;
			bool did_lby_flick, did_predicted_lby_flick;
			bool has_fake;
		};

		/// a snapshot holding info about the moment you shot, used to count shots missed / hit
		struct ShotSnapshot
		{
			sdk::base_entity* entity; /// person we shot at
			player_resolve_record resolve_record; /// their resolve record when we shot

			float time; /// time when snapshot was created
			float first_processed_time; /// time when the shot was first processed
			bool was_shot_processed;
			int hitgroup_hit;
		};

	private:
		player_resolve_record player_resolve_records[ 64 ];
		std::vector<ShotSnapshot> shot_snapshots;
		std::vector<sdk::vec3> last_eye_positions;

	public:
		player_resolve_record& GetPlayerResolveInfo( sdk::base_entity* entity )
		{
			return player_resolve_records[ entity->index( ) ];
		}

		int GetResolveTypeIndex( unsigned short resolve_type );
		int GetShotsMissed( sdk::base_entity* entity, unsigned short resolve_type );

		bool IsResolved( const unsigned short& resolve_type )
		{
			if ( resolve_type & RESOLVE_TYPE_NO_FAKE ||
				resolve_type & RESOLVE_TYPE_LBY_UPDATE ||
				resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE )
				return true;

			return false;
		}

		bool IsFakewalking( sdk::base_entity* entity )
		{
			return player_resolve_records[ entity->index( ) ].is_fakewalking;
		}

		bool IsMovingOnGround( sdk::base_entity* entity )
		{
			return player_resolve_records[ entity->index( ) ].velocity.length_2d( ) > 0.1f && entity->flags( ) & FL_ONGROUND;
		}

		bool IsYawSideways( sdk::base_entity* entity, float yaw );
		bool IsFakingYaw( sdk::base_entity* entity );

		/// pushback a record onto the shot snapshot queue
		void add_shot_snapshot( sdk::base_entity* entity, player_resolve_record resolve_record );

		void event_callback( sdk::game_event* game_event );

	private:
		void ProcessSnapShots( );

		void UpdateResolveRecord( sdk::base_entity* entity );

		void ResolveYaw( sdk::base_entity* entity );
		void ResolvePitch( sdk::base_entity* entity );

		void ResolveYawBruteforce( sdk::base_entity* entity );
		bool AntiFreestanding( sdk::base_entity* entity, float& yaw );
	};

	extern resolver correction;
}
