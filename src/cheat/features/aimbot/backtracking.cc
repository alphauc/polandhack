#include "backtracking.hh"

namespace features::backtracking {
	backtracking backtrack;

	backtracking_record::backtracking_record( sdk::base_entity* ent ) {
		vec_origin = ent->position( );
		abs_angles = ent->abs_angles( );
		eye_angles = ent->eye_angles( );
		render_angles = ent->render_angles( );
		velocity = ent->velocity( );
		simulation_time = ent->simulation_time( );
		flags = ent->flags( );
		animstate = *ent->animstate( );
		//resolve_record = FEATURES::RAGEBOT::resolver.GetPlayerResolveInfo(ent);
		curtime = interfaces::global_vars->cur_time;
		is_exploiting = false;

		for ( int i = 0; i < 15; i++ )
			anim_layers[ i ] = ent->anim_overlay( i );
		for ( int i = 0; i < 128; i++ )
			bone_matrix[ i ] = ent->get_bone_matrix( i );

		memcpy( pose_params, ent->pose_paramter( ), 96 );

		auto collideable = ent->get_collideable( );
		if ( !collideable )
			return;

		bbmin = collideable->mins( );
		bbmax = collideable->maxs( );

		player = ent;
	}

	backtracking_record* backtracking::get_current_record( sdk::base_entity* entity ) {
		if ( !player_records[ entity->index( ) ].size( ) )
			return nullptr;

		return &player_records[ entity->index( ) ][ 0 ];
	}

	void backtracking::store( ) {
		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player || local_player->health( ) <= 0 )
			return;

		for ( int i = 0; i < 64; i++ ) {
			auto entity = interfaces::entity_list->get_entity( i );
			if ( !entity || entity->health( ) <= 0 || entity->team( ) == local_player->team( ) ) {
				player_records[ i ].clear( );
				continue;
			}

			if ( entity->dormant( ) || entity->immune( ) )
				continue;

			set_interpolation_flags( entity, disable_interpolation );
			entity->update_clientside_animations( );

			if ( !player_records[ i ].size( ) ) {
				player_records[ i ].insert( player_records[ i ].begin( ), backtracking_record( entity ) );
				continue;
			}

			// only add if simtime updates, so when player sends data
			if ( player_records[ i ][ 0 ].simulation_time != entity->simulation_time( ) ) {
				auto record = backtracking_record( entity );

				record.previous_vec_origin = player_records[ i ][ 0 ].vec_origin;
				record.previous_simulation_time = player_records[ i ][ 0 ].simulation_time;
				record.previous_curtime = player_records[ i ][ 0 ].curtime;

				player_records[ i ].insert( player_records[ i ].begin( ), record );
			}

			// if more than 0.2 second worth of records (because of the new volvo update lmaohh1337420) pop back the last record 
			if ( interfaces::global_vars->cur_time - player_records[ i ].back( ).curtime > MAX_UNLAG )
				player_records[ i ].pop_back( );
		}
	}

	void backtracking::restore( sdk::base_entity* entity, backtracking_record rec, int extrapolate_ticks ) {
		needs_to_restore[ entity->index( ) ] = true;

		auto extrapolation_amount = sdk::vec3( 0, 0, 0 );
		if ( extrapolate_ticks != 0 ) {
			const auto acceleration = sdk::vec3( 0, 0, 0 );
			const float delta_time = ( rec.curtime - rec.previous_curtime );
			auto velocity = ( rec.vec_origin - rec.previous_vec_origin ) / ( delta_time > 0.f ? delta_time : 1.f );

			while ( extrapolate_ticks > 0 ) {
				extrapolation_amount += velocity * interfaces::global_vars->interval_per_tick;
				velocity += acceleration * interfaces::global_vars->interval_per_tick;

				extrapolate_ticks--;
			}
			while ( extrapolate_ticks < 0 ) {
				extrapolation_amount += velocity * interfaces::global_vars->interval_per_tick;
				velocity += acceleration * interfaces::global_vars->interval_per_tick;

				extrapolate_ticks++;
			}
		}

		memcpy( entity->pose_paramter( ), rec.pose_params, 96 );
		memcpy( entity->animstate( ), &rec.animstate, sizeof( sdk::base_anim_state ) );

		for ( int i = 0; i < 15; i++ )
			entity->set_anim_overlay( i, rec.anim_layers[ i ] );

		auto collideable = entity->get_collideable( );
		if ( !collideable )
			return;

		entity->set_eye_angles( rec.eye_angles );
		entity->set_abs_angles( sdk::vec3( 0, rec.abs_angles.y, 0 ) );
		entity->set_render_angles( rec.render_angles );
		entity->set_abs_origin( rec.vec_origin + extrapolation_amount );
		entity->set_velocity( rec.velocity );
		entity->set_flags( rec.flags );

		collideable->mins( ) = rec.bbmin;
		collideable->maxs( ) = rec.bbmax;
	}

	void backtracking::apply_restore( sdk::base_entity* entity, float curtime ) {
		/// pvs fix
		*reinterpret_cast< int* >( uintptr_t( entity ) + 0xA30 ) = interfaces::global_vars->frame_count;
		*reinterpret_cast< int* >( uintptr_t( entity ) + 0xA28 ) = 0;

		invalidate_bone_cache( entity );

		entity->update_clientside_animations( );
		entity->setup_bones( NULL, 128, 0x0007FF00, curtime ); //bone used by anything
	}

	void backtracking::restore_to_current_record( sdk::base_entity* entity ) {
		if ( !player_records[ entity->index( ) ].size( ) || !needs_to_restore[ entity->index( ) ] )
			return;

		auto record = player_records[ entity->index( ) ][ 0 ];

		restore( entity, record );
		apply_restore( entity, interfaces::global_vars->cur_time );

		needs_to_restore[ entity->index( ) ] = false;
	}

	int backtracking::get_tick_count( backtracking_record record ) {
		int ideal_tick = time_to_ticks( record.simulation_time ) + get_lerp_ticks( );

		return ideal_tick;
	}
	void backtracking::set_interpolation_flags( sdk::base_entity * entity, int flag ) {
		auto VarMap = reinterpret_cast< uintptr_t >( entity ) + 36; // tf2 = 20
		auto VarMapSize = *reinterpret_cast< int* >( VarMap + 20 );

		for ( auto index = 0; index < VarMapSize; index++ )
			*reinterpret_cast< uintptr_t* >( *reinterpret_cast< uintptr_t* >( VarMap ) + index * 12 ) = flag;
	}

	std::vector<backtracking_record> backtracking::priority_record( sdk::base_entity* entity ) {
		std::vector<backtracking_record> priority_records;
		const int player_index = entity->index( );

		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player || local_player->health( ) <= 0 || player_records[ player_index ].size( ) <= 0 )
			return priority_records;

		/// try to find all the records where they're resolved		
		std::vector<backtracking_record> resolved_records;
		for ( const auto& record : player_records[ player_index ] ) {
			if ( get_delta_time( record ) >= MAX_BACKTRACK_RANGE && !record.is_exploiting )
				continue;

			if ( ragebot::correction.IsResolved( record.resolve_record.resolve_type ) )
				resolved_records.push_back( record );
		} //

		/// no resolved records found :(
		if ( !resolved_records.size( ) ) {
			for ( const auto& record : player_records[ player_index ] ) {
				if ( get_delta_time( record ) >= MAX_BACKTRACK_RANGE )
					continue;

				if ( record.resolve_record.resolve_type & ragebot::resolver::RESOLVE_TYPE_ANTI_FREESTANDING ) {
					priority_records.push_back( record );
					return priority_records;
				}
			}

			for ( const auto& record : player_records[ player_index ] ) {
				if ( get_delta_time( record ) >= MAX_BACKTRACK_RANGE )
					continue;

				priority_records.push_back( record );
				break;
			}

			return priority_records;
		}

		/// sort through the resolved records (the could be many if they're moving) and get 2 good ones

		/// not enough records to need sorting
		if ( resolved_records.size( ) <= 2 )
			return resolved_records;

		const auto left_yaw = utilities::math::calculate_angle( local_player->position( ), entity->position( ) ).y + 90.f,
			right_yaw = utilities::math::calculate_angle( local_player->position( ), entity->position( ) ).y - 90.f;

		/// try to find one where they're yaw is sideways to us (easier to hit fam)
		backtracking_record sideways_rec;
		float lowest_sideways_delta = 180.f;
		for ( const auto& record : resolved_records ) {
			const float current_delta = utilities::minimum<float>( fabs( utilities::math::normalize_yaw( record.eye_angles.y - left_yaw ) ), fabs( utilities::math::normalize_yaw( record.eye_angles.y - right_yaw ) ) );
			if ( current_delta < lowest_sideways_delta ) {
				lowest_sideways_delta = current_delta;
				sideways_rec = record;
			}
		}

		/// find the farthest away record from the sideways_rec (for diversity, since the sideways_rec could be behind a wall or smthn)
		backtracking_record opposite_rec;
		float highest_delta = 0.f;
		for ( const auto& record : resolved_records ) {
			const float current_delta = fabs( utilities::math::normalize_yaw( record.eye_angles.y - sideways_rec.eye_angles.y ) );
			if ( current_delta > highest_delta ) {
				highest_delta = current_delta;
				opposite_rec = record;
			}
		}

		if ( highest_delta > 45.f ) {
			priority_records.push_back( opposite_rec );
			priority_records.push_back( sideways_rec );
		}
		else /// return the first record and the slowest record
		{
			backtracking_record slowest_record = resolved_records.back( );
			float slowest_speed = slowest_record.velocity.length_2d( );
			for ( const auto& record : resolved_records ) {
				const float speed = record.velocity.length_2d( );
				if ( speed < slowest_speed ) {
					slowest_speed = speed;
					slowest_record = record;
				}
			}

			if ( fabs( slowest_speed - resolved_records.back( ).velocity.length_2d( ) ) < 30.f )
				slowest_record = resolved_records.back( );

			priority_records.push_back( resolved_records.front( ) );
			priority_records.push_back( slowest_record );
		}

		return priority_records;
	}

	std::vector<backtracking_record> backtracking::valid_records( sdk::base_entity* entity ) {
		std::vector<backtracking_record> valid_records;

		for ( const auto& record : player_records[ entity->index( ) ] ) {
			if ( get_delta_time( record ) < MAX_BACKTRACK_RANGE )
				valid_records.push_back( record );
		}

		return valid_records;
	}

	std::vector<backtracking_record> backtracking::get_records( sdk::base_entity* entity ) {
		return player_records[ entity->index( ) ];
	}

	bool backtracking::get_extrapolated_record( sdk::base_entity* entity, backtracking_record& record ) {
		const int player_index = entity->index( );

		if ( !player_records[ player_index ].size( ) )
			return false;

		record = player_records[ player_index ][ 0 ];

		float time_to_extrapolate_to;
		if ( get_delta_time( record ) > MAX_BACKTRACK_RANGE )
			time_to_extrapolate_to = interfaces::global_vars->cur_time;
		else /// breaking lag compensation
			return false;

		if ( !extrapolation( entity, record, time_to_extrapolate_to ) )
			return false;

		return true;
	}

	bool backtracking::extrapolation( sdk::base_entity* entity, backtracking_record& record, float time ) {
		const int player_index = entity->index( );
		const float time_delta = time - record.curtime;

		static auto sv_gravity = interfaces::convar->find_variable( "sv_gravity" );
		static auto sv_jump_impulse = interfaces::convar->find_variable( "sv_jump_impulse" );

		/// need 3 records to extrapolate
		if ( player_records[ player_index ].size( ) < 3 )
			return false;

		/// to check if ground is below the nigger, and speed is per second
		auto IsObjectInWay = [ ]( sdk::vec3 origin, sdk::vec3 velocity, sdk::vec3& end ) -> bool {
			sdk::trace_t trace;
			sdk::trace_world_only filter;
			sdk::ray_t ray;
			ray.initialize( origin, origin + ( velocity * interfaces::global_vars->interval_per_tick ) );

			interfaces::trace->TraceRay( ray, MASK_ALL, &filter, &trace );

			end = trace.end;

			return trace.flFraction < 1.f;
		};

		auto record_1 = player_records[ player_index ][ 0 ], record_2 = player_records[ player_index ][ 1 ], record_3 = player_records[ player_index ][ 2 ];
		record = record_1;

		/// velocity and acceleration are per second, not per tick
		sdk::vec3 velocity = record.velocity;
		sdk::vec3 acceleration = ( ( record_1.velocity - record_2.velocity ) + ( record_2.velocity - record_3.velocity ) ) / ( record_1.simulation_time - record_2.simulation_time );
		acceleration.z = -sv_gravity->GetFloat( );

		bool was_object_in_way_last_tick = false;
		float curtime = record.simulation_time;
		while ( curtime < time ) {
			sdk::vec3 vel_change = velocity * interfaces::global_vars->interval_per_tick;

			record.vec_origin += vel_change;
			record.bbmax += vel_change;
			record.bbmin += vel_change;

			velocity += acceleration * interfaces::global_vars->interval_per_tick;

			sdk::vec3 end;
			if ( IsObjectInWay( record.vec_origin, velocity, end ) ) {
				record.vec_origin = end;

				if ( !was_object_in_way_last_tick )
					velocity.z = sv_jump_impulse->GetFloat( );
				else
					break;

				was_object_in_way_last_tick = true;
			}
			else
				was_object_in_way_last_tick = false;

			curtime += interfaces::global_vars->interval_per_tick;
		}

		return true;
	}

	void backtracking::invalidate_bone_cache( sdk::base_entity* entity ) {
		static uintptr_t InvalidateBoneCache = utilities::find_pattern( "client_panorama.dll", ( PBYTE )"\x80\x3D\x00\x00\x00\x00\x00\x74\x16\xA1", "xx????xxxx" );
		static uintptr_t g_iModelBoneCounter = **( uintptr_t** )( InvalidateBoneCache + 10 ); //	Offsets::InvalidateBoneCache = FindPatternIDA("client.dll", "80 3D ? ? ? ? 00 74 16 A1");
		*( int* )( ( uintptr_t )entity + netvars::get_offset_cached<fnv::hash_constexpr( "CBaseAnimating->m_nForceBone" )>( ) + 0x20 ) = 0; //m_nForceBone + 0x20 //might be wrong..
		*( uintptr_t* )( ( uintptr_t )entity + 0x2914 ) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
		*( uintptr_t* )( ( uintptr_t )entity + 0x2680 ) = ( g_iModelBoneCounter - 1 ); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
	}

	float backtracking::get_delta_time( backtracking_record record ) {
		auto nci = interfaces::engine_client->channel_info( );
		if ( !nci )
			return FLT_MAX;

		float correct = 0.f;
		correct += nci->GetLatency( FLOW_OUTGOING );
		correct += nci->GetLatency( FLOW_INCOMING );
		correct += ticks_to_time( get_lerp_ticks( ) );
		correct = utilities::clamp<float>( correct, 0.f, 1.f );

		return fabs( correct - ( utilities::curtime( ) - record.simulation_time ) );
	}

	int backtracking::fix_tick_count( sdk::base_entity * player, backtracking_record& record ) {
		if ( record.simulation_time == 0.0f || !is_tick_valid( record.simulation_time ) )
			return time_to_ticks( player->simulation_time( ) + get_lerp_ticks( ) );

		return time_to_ticks( record.simulation_time + get_lerp_ticks( ) );
	}

	bool backtracking::is_tick_valid( int tick ) {
		// better use polak's version than our old one, getting more accurate results

		auto nci = interfaces::engine_client->channel_info( );;

		static auto sv_maxunlag = interfaces::convar->find_variable( "sv_maxunlag" );

		if ( !nci || !sv_maxunlag )
			return false;

		float correct = utilities::clamp<float>( nci->GetLatency( FLOW_OUTGOING ) + get_lerp_ticks( ), 0.f, sv_maxunlag->GetFloat( ) );

		float deltaTime = correct - ( utilities::curtime( ) - ticks_to_time( tick ) );

		return fabsf( deltaTime ) < 0.2f;
	}

	int backtracking::get_lerp_ticks( ) {
		static const auto cl_interp_ratio = interfaces::convar->find_variable( "cl_interp_ratio" );
		static const auto cl_updaterate = interfaces::convar->find_variable( "cl_updaterate" );
		static const auto cl_interp = interfaces::convar->find_variable( "cl_interp" );

		return time_to_ticks( max( cl_interp->GetFloat( ), cl_interp_ratio->GetFloat( ) / cl_updaterate->GetFloat( ) ) );
	}
}