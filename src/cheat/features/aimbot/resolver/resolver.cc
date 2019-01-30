#include "resolver.hh"
#include "../backtracking.hh"
#include "../autowall/autowall.hh"

#define ttime_to_ticks( dt )		( (int)( 0.5 + (float)(dt) / interfaces::global_vars->interval_per_tick ) )
#define tticks_to_time( t )		( interfaces::global_vars->interval_per_tick *( t ) )

constexpr unsigned int fnv_hash_exact( const char* const data, const unsigned int value = 2166136261 )
{
	return ( data[ 0 ] == '\0' ) ? value : ( fnv_hash_exact( &data[ 1 ], ( value * 16777619 ) ^ data[ 0 ] ) );
}


namespace features::ragebot {
	resolver correction;

	resolver::resolver( ) { }

	void resolver::execute_frame_stage_notify( ) {
		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player || local_player->health( ) <= 0 )
			return;

		for ( int i = 0; i < 64; i++ ) {
			auto entity = interfaces::entity_list->get_entity( i );
			if ( !entity || entity->health( ) <= 0 || entity->team( ) == local_player->team( ) )
				continue;

			UpdateResolveRecord( entity );

			/// make sure to do dormant check AFTER calling UpdateResolveRecord()
			if ( entity->dormant( ) )
				continue;

			ResolveYaw( entity );
			ResolvePitch( entity );

			const auto resolve_record = player_resolve_records[ i ];
			entity->set_eye_angles( resolve_record.resolved_angles );
		}
	}

	void resolver::execute_create_move( ) {
		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player || local_player->health( ) <= 0 )
			return;

		ProcessSnapShots( );

		last_eye_positions.insert( last_eye_positions.begin( ), local_player->position( ) + local_player->view_offset( ) );
		if ( last_eye_positions.size( ) > 128 )
			last_eye_positions.pop_back( );

		auto nci = interfaces::engine_client->channel_info( );
		if ( !nci )
			return;

		const int latency_ticks = ttime_to_ticks( nci->GetLatency( FLOW_OUTGOING ) );
		const auto latency_based_eye_pos = last_eye_positions.size( ) <= latency_ticks ? last_eye_positions.back( ) : last_eye_positions[ latency_ticks ];

		for ( int i = 0; i < 64; i++ ) {
			auto entity = interfaces::entity_list->get_entity( i );
			if ( !entity || entity->health( ) <= 0 || entity->team( ) == local_player->team( ) || entity->dormant( ) )
				continue;

			auto& resolve_record = player_resolve_records[ i ];

			const float height = 64;

			sdk::vec3 direction_1, direction_2;
			utilities::math::angle_vectors( sdk::vec3( 0.f, utilities::math::calculate_angle(
				local_player->position( ), entity->position( ) ).y - 90.f, 0.f ), &direction_1 );

			utilities::math::angle_vectors( sdk::vec3( 0.f, utilities::math::calculate_angle(
				local_player->position( ), entity->position( ) ).y + 90.f, 0.f ), &direction_2 );

			const auto left_eye_pos = entity->position( ) + sdk::vec3( 0, 0, height ) + ( direction_1 * 16.f );
			const auto right_eye_pos = entity->position( ) + sdk::vec3( 0, 0, height ) + ( direction_2 * 16.f );

			resolve_record.anti_freestanding_record.left_damage = autowall.CalculateDamage( latency_based_eye_pos,
				left_eye_pos, local_player, entity, 1 ).damage;
			resolve_record.anti_freestanding_record.right_damage = autowall.CalculateDamage( latency_based_eye_pos,
				right_eye_pos, local_player, entity, 1 ).damage;

			sdk::ray_t ray;
			sdk::trace_t trace;
			sdk::trace_world_only filter;

			ray.initialize( left_eye_pos, latency_based_eye_pos );
			interfaces::trace->TraceRay( ray, MASK_ALL, &filter, &trace );
			resolve_record.anti_freestanding_record.left_fraction = trace.flFraction;

			ray.initialize( right_eye_pos, latency_based_eye_pos );
			interfaces::trace->TraceRay( ray, MASK_ALL, &filter, &trace );
			resolve_record.anti_freestanding_record.right_fraction = trace.flFraction;

			resolve_record.did_predicted_lby_flick = false;
			resolve_record.did_lby_flick = false;
		}
	}

	void resolver::add_shot_snapshot( sdk::base_entity* entity, player_resolve_record resolve_record ) {
		ShotSnapshot snapshot;

		snapshot.entity = entity;
		snapshot.time = utilities::curtime( );
		snapshot.resolve_record = resolve_record;
		snapshot.first_processed_time = 0.f;
		snapshot.was_shot_processed = false;
		snapshot.hitgroup_hit = -1;

		shot_snapshots.push_back( snapshot );
	}

	void resolver::event_callback( sdk::game_event* game_event ) {
		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player || local_player->health( ) <= 0 || !game_event )
			return;

		if ( shot_snapshots.size( ) <= 0 )
			return;

		auto& snapshot = shot_snapshots.front( );
		switch ( fnv_hash_exact( game_event->GetName( ) ) ) {
		case fnv_hash_exact( "player_hurt" ):
		{
			/// it wasn't us who shot
			if ( interfaces::engine_client->get_player_for_user_id( game_event->GetInt( "attacker" ) ) != interfaces::engine_client->get_local_player( ) )
				return;

			if ( !snapshot.was_shot_processed ) {
				snapshot.was_shot_processed = true;
				snapshot.first_processed_time = utilities::curtime( );
			}

			snapshot.hitgroup_hit = game_event->GetInt(  "hitgroup" );

			break;
		}
		case fnv_hash_exact( "bullet_impact" ):
		case fnv_hash_exact( "weapon_fire" ):
		{
			/// it wasn't us who shot
			if ( interfaces::engine_client->get_player_for_user_id( game_event->GetInt( "userid" ) ) != interfaces::engine_client->get_local_player( ) )
				return;

			if ( !snapshot.was_shot_processed ) {
				snapshot.was_shot_processed = true;
				snapshot.first_processed_time = utilities::curtime( );
			}

			break;
		}
		}
	}

	void resolver::ProcessSnapShots( ) {
		if ( shot_snapshots.size( ) <= 0 )
			return;

		const auto snapshot = shot_snapshots.front( );
		if ( fabs( utilities::curtime( ) - snapshot.time ) > 1.f ) {
			shot_snapshots.erase( shot_snapshots.begin( ) );
			return;
		}

		const int player_index = snapshot.entity->index( );
		if ( snapshot.hitgroup_hit != -1 ) /// hit
		{
			/// increment shots fired and shots hit
			for ( int i = 0; i < RESOLVE_TYPE_NUM; i++ ) {
				if ( snapshot.resolve_record.resolve_type & ( 1 << i ) ) {
					player_resolve_records[ player_index ].shots_fired[ i ]++;
					player_resolve_records[ player_index ].shots_hit[ i ]++;
				}
			}
		}
		else if ( snapshot.first_processed_time != 0.f && fabs( utilities::curtime( ) - snapshot.first_processed_time ) > 0.1f ) /// missed
		{
			/// increment shots fired but not shots hit
			for ( int i = 0; i < RESOLVE_TYPE_NUM; i++ ) {
				if ( snapshot.resolve_record.resolve_type & ( 1 << i ) )
					player_resolve_records[ player_index ].shots_fired[ i ]++;
			}

			if ( snapshot.resolve_record.resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY )
				player_resolve_records[ player_index ].shots_missed_moving_lby++;

			if ( snapshot.resolve_record.resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA )
				player_resolve_records[ player_index ].shots_missed_moving_lby_delta++;
		}
		else
			return;

		shot_snapshots.erase( shot_snapshots.begin( ) );
	}

	void resolver::UpdateResolveRecord( sdk::base_entity* entity ) {
		/// a copy of the previous record for comparisons and shit
		const auto previous_record = player_resolve_records[ entity->index( ) ];
		auto& record = player_resolve_records[ entity->index( ) ];

		record.resolved_angles = record.networked_angles;
		record.velocity = entity->velocity( );
		record.origin = entity->position( );
		record.lower_body_yaw = entity->lower_body_yaw( );
		record.is_dormant = entity->dormant( );
		record.has_fake = IsFakingYaw( entity );
		record.resolve_type = 0;

		record.is_balance_adjust_triggered = false, record.is_balance_adjust_playing = false;

		/* I'm honestly just too fucking lazy, please let me off. ~alpha
				for ( int i = 0; i < 15; i++ ) /// go through each animation layer
		{
			record.anim_layers[ i ] = entity->anim_overlay( i );

			/// balanceadjust act
			if ( entity->GetSequenceActivity( record.anim_layers[ i ].m_nSequence ) == SDK::CSGO_ACTS::ACT_CSGO_IDLE_TURN_BALANCEADJUST ) {
				record.is_balance_adjust_playing = true;

				/// balance adjust is being triggered
				if ( record.anim_layers[ i ].m_flWeight == 1 || record.anim_layers[ i ].m_flCycle > previous_record.anim_layers[ i ].m_flCycle )
					record.last_balance_adjust_trigger_time = utilities::curtime( );
				if ( fabs( utilities::curtime( ) - record.last_balance_adjust_trigger_time ) < 0.5f )
					record.is_balance_adjust_triggered = true;
			}
		}

		*/

		if ( record.is_dormant )
			record.next_predicted_lby_update = FLT_MAX;

		/// if lby updated
		if ( record.lower_body_yaw != previous_record.lower_body_yaw && !record.is_dormant && !previous_record.is_dormant )
			record.did_lby_flick = true;

		/// fakewalk
		const bool is_moving_on_ground = record.velocity.length_2d( ) > 50 && entity->flags( ) & FL_ONGROUND;
		if ( is_moving_on_ground && record.is_balance_adjust_triggered )
			record.is_fakewalking = true;
		else
			record.is_fakewalking = false;

		/// last moving lby, using a high velocity check ***just in case*** of fakewalkers
		if ( is_moving_on_ground && !record.is_fakewalking && record.velocity.length_2d( ) > 1.f && !record.is_dormant ) {
			record.is_last_moving_lby_valid = true;
			record.is_last_moving_lby_delta_valid = false;
			record.shots_missed_moving_lby = 0;
			record.shots_missed_moving_lby_delta = 0;
			record.last_moving_lby = record.lower_body_yaw + 45;
			record.last_time_moving = utilities::curtime( );
		}

		/// just came out of dormancy
		if ( !record.is_dormant && previous_record.is_dormant ) {
			/// if moved more than 32 units
			if ( ( record.origin - previous_record.origin ).length_2d( ) > 16.f )
				record.is_last_moving_lby_valid = false;
		}

		/// get last moving lby delta
		if ( !record.is_last_moving_lby_delta_valid && record.is_last_moving_lby_valid && record.velocity.length_2d( ) < 20 && fabs( utilities::curtime( ) - record.last_time_moving ) < 1.0 ) {
			/// if lby updated
			if ( record.lower_body_yaw != previous_record.lower_body_yaw ) {
				record.last_moving_lby_delta = utilities::math::normalize_yaw( record.last_moving_lby - record.lower_body_yaw );
				record.is_last_moving_lby_delta_valid = true;
			}
		}

		if ( utilities::math::normalize_pitch( record.networked_angles.x ) > 5.f )
			record.last_time_down_pitch = utilities::curtime( );
	}

	void resolver::ResolveYaw( sdk::base_entity* entity ) {
		auto& resolve_record = player_resolve_records[ entity->index( ) ];

		if ( resolve_record.did_lby_flick ) {
			resolve_record.resolved_angles.y = resolve_record.lower_body_yaw;
			resolve_record.resolve_type |= RESOLVE_TYPE_LBY_UPDATE;
		}
		else if ( resolve_record.did_predicted_lby_flick ) {
			resolve_record.resolved_angles.y = resolve_record.lower_body_yaw;
			resolve_record.resolve_type |= RESOLVE_TYPE_PREDICTED_LBY_UPDATE;
		}
		else if ( const auto bt_recs = backtracking::backtrack.get_records( entity ); bt_recs.size( ) >= 1 && !bt_recs.front( ).resolve_record.has_fake && !IsFakingYaw( entity ) ) {
			resolve_record.resolved_angles.y = resolve_record.networked_angles.y;
			resolve_record.resolve_type |= RESOLVE_TYPE_NO_FAKE;
		}
		else if ( IsMovingOnGround( entity ) && !IsFakewalking( entity ) ) /// moving
		{
			resolve_record.resolved_angles.y = resolve_record.lower_body_yaw;
			resolve_record.resolve_type |= RESOLVE_TYPE_LBY;
		}
		else if ( resolve_record.is_last_moving_lby_valid && resolve_record.shots_missed_moving_lby < 1 && IsYawSideways( entity, resolve_record.last_moving_lby ) ) /// last moving lby if sideways prioritizes over antiurine
		{
			resolve_record.resolved_angles.y = resolve_record.last_moving_lby;
			resolve_record.resolve_type |= RESOLVE_TYPE_LAST_MOVING_LBY;
		}
		else if ( AntiFreestanding( entity, resolve_record.resolved_angles.y ) ) {
			resolve_record.resolve_type |= RESOLVE_TYPE_ANTI_FREESTANDING;
		}
		else if ( resolve_record.is_last_moving_lby_valid && resolve_record.shots_missed_moving_lby < 1 ) /// last moving lby
		{
			resolve_record.resolved_angles.y = resolve_record.last_moving_lby;
			resolve_record.resolve_type |= RESOLVE_TYPE_LAST_MOVING_LBY;
		}
		else /// bruteforce as last resort
			ResolveYawBruteforce( entity );
	}

	void resolver::ResolvePitch( sdk::base_entity* entity ) {
		auto& resolve_record = player_resolve_records[ entity->index( ) ];

		if ( resolve_record.resolve_type & RESOLVE_TYPE_LBY_UPDATE || resolve_record.resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE ||
			resolve_record.resolve_type & RESOLVE_TYPE_NO_FAKE )
			resolve_record.resolved_angles.x = resolve_record.networked_angles.x;
		else {
			/// super fucking ghetto fix to stop their pitch from going to 0 when they're shooting (only neccesary when they're using psilent)
			if ( fabs( utilities::curtime( ) - resolve_record.last_time_down_pitch ) < 0.5f )
				resolve_record.resolved_angles.x = 70.f;
			else
				resolve_record.resolved_angles.x = resolve_record.networked_angles.x;
		}

		resolve_record.resolved_angles.x = utilities::math::normalize_pitch( resolve_record.resolved_angles.x );
	}

	void resolver::ResolveYawBruteforce( sdk::base_entity* entity ) {
		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player )
			return;

		auto& resolve_record = player_resolve_records[ entity->index( ) ];
		resolve_record.resolve_type |= RESOLVE_TYPE_BRUTEFORCE;

		const float at_target_yaw = utilities::math::calculate_angle( entity->position( ), local_player->position( ) ).y;

		const int shots_missed = resolve_record.shots_fired[ GetResolveTypeIndex( resolve_record.resolve_type ) ] -
			resolve_record.shots_hit[ GetResolveTypeIndex( resolve_record.resolve_type ) ];
		switch ( shots_missed % 3 ) {
		case 0:
			resolve_record.resolved_angles.y = utilities::math::rotated_lower_body_yaw( entity->lower_body_yaw( ), at_target_yaw + 60.f );
			break;
		case 1:
			resolve_record.resolved_angles.y = at_target_yaw + 140.f;
			break;
		case 2:
			resolve_record.resolved_angles.y = at_target_yaw - 75.f;
			break;
		}
	}

	bool resolver::AntiFreestanding( sdk::base_entity* entity, float& yaw ) {
		const auto freestanding_record = player_resolve_records[ entity->index( ) ].anti_freestanding_record;

		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player )
			return false;

		if ( freestanding_record.left_damage >= 20 && freestanding_record.right_damage >= 20 )
			return false;

		const float at_target_yaw = utilities::math::calculate_angle( local_player->position( ), entity->position( ) ).y;
		if ( freestanding_record.left_damage <= 0 && freestanding_record.right_damage <= 0 ) {
			if ( freestanding_record.right_fraction < freestanding_record.left_fraction )
				yaw = at_target_yaw + 125.f;
			else
				yaw = at_target_yaw - 73.f;
		}
		else {
			if ( freestanding_record.left_damage > freestanding_record.right_damage )
				yaw = at_target_yaw + 130.f;
			else
				yaw = at_target_yaw - 49.f;
		}

		return true;
	}

	bool resolver::IsYawSideways( sdk::base_entity* entity, float yaw ) {
		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player )
			return false;

		const auto at_target_yaw = utilities::math::calculate_angle( local_player->position( ), entity->position( ) ).y;
		const float delta = fabs( utilities::math::normalize_yaw( at_target_yaw - yaw ) );

		return delta > 20.f && delta < 160.f;
	}

	bool resolver::IsFakingYaw( sdk::base_entity* entity ) {
		const auto bt_records = backtracking::backtrack.get_records( entity );
		if ( bt_records.size( ) < 2 )
			return true;

		if ( bt_records.front( ).is_exploiting )
			return true;

		if ( fabs( bt_records[ 0 ].simulation_time - bt_records[ 1 ].simulation_time ) == interfaces::global_vars->interval_per_tick )
			return false;

		return true;
	}

	int resolver::GetResolveTypeIndex( unsigned short resolve_type ) {
		/// gonna have to use lorge if statements cuz fuck you
		if ( resolve_type & RESOLVE_TYPE_OVERRIDE )
			return 0;
		else if ( resolve_type & RESOLVE_TYPE_NO_FAKE )
			return 1;
		else if ( resolve_type & RESOLVE_TYPE_LBY )
			return 2;
		else if ( resolve_type & RESOLVE_TYPE_LBY_UPDATE )
			return 3;
		else if ( resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE )
			return 4;
		else if ( resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY )
			return 5;
		else if ( resolve_type & RESOLVE_TYPE_NOT_BREAKING_LBY )
			return 6;
		else if ( resolve_type & RESOLVE_TYPE_BRUTEFORCE )
			return 7;
		else if ( resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA )
			return 8;
		else if ( resolve_type & RESOLVE_TYPE_ANTI_FREESTANDING )
			return 9;

		return 0;
	}

	int resolver::GetShotsMissed( sdk::base_entity* entity, unsigned short resolve_type ) {
		const int resolve_index = GetResolveTypeIndex( resolve_type );

		return player_resolve_records[ entity->index( ) ].shots_fired[ resolve_index ] - player_resolve_records[ entity->index( ) ].shots_hit[ resolve_index ];
	}

}
