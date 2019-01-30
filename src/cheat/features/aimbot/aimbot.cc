#include "aimbot.hh"
#include "autowall/autowall.hh"
#include "../../cfg.hh"
#define nahhh false

#define M_PI_F		((float)(M_PI))	// DFGHACXGNSBGFHSDFSGHTJFSGHJYYRTJERTHWRWTYHJRSYJRTWSHRTYGWJN
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )

namespace features::ragebot {
	automatic_shoot_at_target_bot aimbot;

	void automatic_shoot_at_target_bot::execute( sdk::user_cmd* cmd ) {
		csgo::can_shoot_someone = false;

		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		auto weapon = ( sdk::base_weapon* )interfaces::entity_list->get_entity( local_player->active_weapon_handle( ) );
		if ( !local_player || local_player->health( ) <= 0 || !g_config.aimbot.enabled )
			return;

		auto BoundingBoxCheck = [ this, local_player ]( sdk::base_entity* entity, const backtracking::backtracking_record& record ) -> bool {
			const auto bbmin = record.bbmin + record.vec_origin;
			const auto bbmax = record.bbmax + record.vec_origin;

			/// the 4 corners on the top, 1 for the head, 1 for the middle of the body, 1 for the feet
			sdk::vec3 points[ 7 ];

			points[ 0 ] = GetHitboxPosition( entity, 0 );
			points[ 1 ] = ( bbmin + bbmax ) * 0.5f;
			points[ 2 ] = sdk::vec3( ( bbmax.x + bbmin.x ) * 0.5f, ( bbmax.y + bbmin.y ) * 0.5f, bbmin.z );

			points[ 3 ] = bbmax;
			points[ 4 ] = sdk::vec3( bbmax.x, bbmin.y, bbmax.z );
			points[ 5 ] = sdk::vec3( bbmin.x, bbmin.y, bbmax.z );
			points[ 6 ] = sdk::vec3( bbmin.x, bbmax.y, bbmax.z );

			for ( const auto& point : points ) {
				if ( autowall.CalculateDamage( local_player->position( ) + local_player->view_offset( ), point, local_player, entity ).damage > 0 )
					return true;
			}

			return false;
		};

		/// loop through every entity to find valid ones to aimbot
		for ( int i = 0; i < 64; i++ ) {
			/// reset player aimbot info
			player_aimbot_info[ i ].head_damage = -1, player_aimbot_info[ i ].hitscan_damage = -1;

			auto entity = interfaces::entity_list->get_entity( i );
			if ( !entity || entity->dormant( ) || entity->immune( ) || entity->health( ) <= 0 || entity->team( ) == local_player->team( ) )
				continue;

			int most_damage = 0;
			for ( const auto& rec : backtracking::backtrack.priority_record( entity ) ) {
				const int tick = backtracking::backtrack.get_tick_count( rec );

				/// optimization
				if ( !BoundingBoxCheck( entity, rec ) )
					continue;

				int random_ticks = 0;
				if ( rec.is_exploiting )
					random_ticks = time_to_ticks( utilities::latency( ) );

				backtracking::backtrack.restore( entity, rec, random_ticks );
				backtracking::backtrack.apply_restore( entity, time_to_ticks( tick ) );

				int cur_head_damage, cur_hitscan_damage;
				sdk::vec3 cur_head_pos, cur_hitscan_pos;

				DoHeadAim( entity, cur_head_pos, cur_head_damage );
				DoHitscan( entity, cur_hitscan_pos, cur_hitscan_damage );

				if ( cur_head_damage > most_damage || cur_hitscan_damage > most_damage ) {
					most_damage = utilities::maximum<int>( cur_head_damage, cur_hitscan_damage );

					player_aimbot_info[ i ].head_damage = cur_head_damage;
					player_aimbot_info[ i ].hitscan_damage = cur_hitscan_damage;

					player_aimbot_info[ i ].head_position = cur_head_pos;
					player_aimbot_info[ i ].hitscan_position = cur_hitscan_pos;

					player_aimbot_info[ i ].tick = tick;
					player_aimbot_info[ i ].backtrack_record = rec;
					player_aimbot_info[ i ].extrapolted_ticks = random_ticks;

					if ( cur_hitscan_damage > entity->health( ) )
						break;
				}
			}
		}

		int highest_damage = 0, best_tick, last_extrapolated_ticks;
		backtracking::backtracking_record last_backtrack_record;
		sdk::base_entity* best_entity = nullptr;
		bool is_hitscan = false;
		sdk::vec3 best_position;

		/// sort valid entities by most damage
		for ( int i = 0; i < 64; i++ ) {
			auto entity = interfaces::entity_list->get_entity( i );
			if ( !entity )
				continue;

			auto FillInfo = [ &is_hitscan, &best_position, &best_entity, &last_backtrack_record, &last_extrapolated_ticks, &highest_damage, &best_tick, entity, i, this ]( bool hitscan ) -> void {
				is_hitscan = hitscan, best_tick = player_aimbot_info[ i ].tick;
				best_entity = entity, last_backtrack_record = player_aimbot_info[ i ].backtrack_record;
				last_extrapolated_ticks = player_aimbot_info[ i ].extrapolted_ticks;

				if ( hitscan ) {
					best_position = player_aimbot_info[ i ].hitscan_position;
					highest_damage = player_aimbot_info[ i ].hitscan_damage;
				}
				else {
					best_position = player_aimbot_info[ i ].head_position;
					highest_damage = player_aimbot_info[ i ].head_damage;
				}
			};
			const int hitscan_dmg = player_aimbot_info[ i ].hitscan_damage, head_dmg = player_aimbot_info[ i ].head_damage;

			/// if exploiting or jumping force baim and entirely ignore head
			const bool is_exploiting = player_aimbot_info[ i ].backtrack_record.is_exploiting;
			if ( is_exploiting || ( !( entity->flags( ) & FL_ONGROUND ) ) ) {
				if ( hitscan_dmg > highest_damage )
					FillInfo( true );

				continue;
			}

			/// can't shoot this entity, continue
			if ( hitscan_dmg <= 0 && head_dmg <= 0 )
				continue;

			/// one of these is not valid, choose the one that is valid
			if ( hitscan_dmg <= 0 || head_dmg <= 0 ) {
				FillInfo( hitscan_dmg > head_dmg );
				continue;
			}

			/// hitscan if lethal or if more than head damage
			if ( hitscan_dmg > entity->health( ) || hitscan_dmg > head_dmg ) {
				FillInfo( true );
				if ( hitscan_dmg > entity->health( ) )
					break;

				continue;
			}

			/// if resolved go for head
			if ( correction.IsResolved( player_aimbot_info[ i ].backtrack_record.resolve_record.resolve_type ) ) {
				FillInfo( false );
				break;
			}

			const bool baim_if_unplugged = ( g_config.aimbot.baim_if_unplugged && fabs( utilities::curtime( ) - player_aimbot_info[ i ].backtrack_record.resolve_record.last_time_moving ) > 1.f );
			const bool prefer_baim = g_config.aimbot.baim_always || (!( entity->flags( ) & FL_ONGROUND ) && g_config.aimbot.baim_if_air) || ( g_config.aimbot.baim_if_x_damage_enabled && g_config.aimbot.baim_if_x_damage < player_aimbot_info[ i ].hitscan_damage );
			if ( prefer_baim || baim_if_unplugged ) {
				FillInfo( true );

				continue;
			}

			FillInfo( false );
		}

		static float duration_spent_aiming = 0.f;

		/// we have a valid target to shoot at
		if ( best_entity ) {
			csgo::can_shoot_someone = true;

			duration_spent_aiming += interfaces::global_vars->interval_per_tick;

			/// maybe implement a knifebot sometime in the future?
			const bool should_right_click = false;

			/// the weapon is ready to fire
			if ( utilities::can_shoot( should_right_click ) ) {

				/// need to restore to the player's backtrack record so that hitchance can work properly
				backtracking::backtrack.restore( best_entity, last_backtrack_record, last_extrapolated_ticks );
				backtracking::backtrack.apply_restore( best_entity, best_tick );

				/// delay shot
				auto DelayShot = [ best_entity, last_backtrack_record, highest_damage, local_player, is_hitscan ]( ) -> bool {
					/// jumping
					if ( !( best_entity->flags( ) & FL_ONGROUND ) )
						return true;

					if ( highest_damage >= best_entity->health( ) + 10 )
						return true;

					if ( best_entity->velocity( ).length_2d( ) > 75.f || local_player->velocity( ).length_2d( ) > 75.f ) /// moving fast
						return duration_spent_aiming >= g_config.aimbot.delay_shot;

					return true;
				};
				if ( DelayShot( ) ) {
					/// if is accurate enough to shoot, then shoot... (hitchance, spread limit, etc)
					if ( const auto net_channel = interfaces::client_state->net_channel; IsAccurate( best_entity, best_position ) && net_channel && net_channel->choked_packets ) {

						if ( should_right_click )
							cmd->buttons |= IN_ATTACK2;
						else
							cmd->buttons |= IN_ATTACK;

						cmd->view_angles = utilities::math::calculate_angle( local_player->position( ) + local_player->view_offset( ), best_position ) - ( local_player->punch_angles( ) * 2.f );
						cmd->tick_count = best_tick;

						/// resolver
						auto IsSidewaysAntiaim = [ is_hitscan, best_entity, local_player, last_backtrack_record ]( ) -> bool {
							const float at_target_yaw = utilities::math::normalize_yaw( utilities::math::calculate_angle( local_player->position( ), best_entity->position( ) ).y );

							const float delta_1 = fabs( utilities::math::normalize_yaw( ( at_target_yaw + 90.f ) - last_backtrack_record.eye_angles.y ) ),
								delta_2 = fabs( utilities::math::normalize_yaw( ( at_target_yaw - 90.f ) - last_backtrack_record.eye_angles.y ) );

							return utilities::minimum<float>( delta_1, delta_2 ) > 30.f;
						};
						if ( !is_hitscan || IsSidewaysAntiaim( ) )
							correction.add_shot_snapshot( best_entity, correction.GetPlayerResolveInfo( best_entity ) );

						/// psilent
						csgo::send_packet = false;
					}
				}
			}
		}

		/// restore all players back to their current record
		for ( int i = 0; i < 64; i++ ) {
			auto entity = interfaces::entity_list->get_entity( i );
			if ( !entity || entity->dormant( ) || entity->immune( ) || entity->health( ) <= 0 || entity->team( ) == local_player->team( ) )
				continue;

			backtracking::backtrack.restore_to_current_record( entity );
		}

		if ( !best_entity )
			duration_spent_aiming = 0.f;
	}

	bool automatic_shoot_at_target_bot::DoHitscan( sdk::base_entity* entity, sdk::vec3& final_position, int& final_damage ) {
		/// default value
		final_damage = 0;

		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player )
			return false;

		auto weapon = ( sdk::base_weapon* )interfaces::entity_list->get_entity( local_player->active_weapon_handle( ) );
		if ( !weapon )
			return false;

		const auto eye_position = local_player->position( ) + local_player->view_offset( );
		const bool is_taser = nahhh;
		const bool should_ignore_limbs = entity->velocity( ).length_2d( ) > 30.f;
		const float pointscale = g_config.aimbot.pointscale / 100;

		/// if taser or low on ammo, set minimum damage to their health so that it kills in a single shot
		const int minimum_damage = ( is_taser || weapon->clip1_count( ) <= 2 ) ? entity->health( ) + 10 : utilities::minimum<int>( g_config.aimbot.min_dmg, entity->health( ) + 10 );
		const int minimum_autowall_damage = ( is_taser || weapon->clip1_count( ) <= 2 ) ? entity->health( ) + 10 : utilities::minimum<int>(g_config.aimbot.min_awall_dmg, entity->health( ) + 10 );

		/// functions for creating the hitscan positions
		auto ConstructHitscanMultipointPositions = [ this, entity, pointscale, eye_position, local_player ]( bool ignore_limbs ) -> std::vector<std::pair<sdk::vec3, int>> {
			std::vector<std::pair<sdk::vec3, int>> positions;

			auto CreateAndAddBufferToVec = [ this, entity ]( std::vector<std::pair<sdk::vec3, int>>& vec, int hitbox_index, float pointscale ) {
				std::vector<sdk::vec3> buffer;
				GetMultipointPositions( entity, buffer, hitbox_index, pointscale );

				for ( const auto& x : buffer )
					vec.emplace_back( x, hitbox_index );
			};
			auto GetSpreadDistance = [ this, local_player ]( float distance ) -> float {
				auto weapon = ( sdk::base_weapon* )interfaces::entity_list->get_entity( local_player->active_weapon_handle( ) );
				if ( !weapon )
					return 0.f;

				sdk::vec3 viewangles =
					interfaces::engine_client->get_view_angles( );

				weapon->update_accuraty_penalty( );
				float spread = weapon->spread_cone( );
				float inaccuracy = weapon->inaccuracy( );

				sdk::vec3 forward, right, up;
				utilities::math::angle_vectors( viewangles, &forward, &right, &up );

				float RandomA = 0;
				float RandomB = 1.0f - RandomA * RandomA;

				RandomA = M_PI_F * 2.0f;
				RandomB *= spread;

				float SpreadX1 = ( cos( RandomA ) * RandomB );
				float SpreadY1 = ( sin( RandomA ) * RandomB );

				float RandomC = 0;
				float RandomF = 1.0f - RandomC * RandomC;

				RandomC = M_PI_F * 2.0f;
				RandomF *= inaccuracy;

				float SpreadX2 = ( cos( RandomC ) * RandomF );
				float SpreadY2 = ( sin( RandomC ) * RandomF );

				float fSpreadX = SpreadX1 + SpreadX2;
				float fSpreadY = SpreadY1 + SpreadY2;

				sdk::vec3 vSpreadForward;
				vSpreadForward.x = forward.x + ( fSpreadX * right.x ) + ( fSpreadY * up.x );
				vSpreadForward.y = forward.y + ( fSpreadX * right.y ) + ( fSpreadY * up.y );
				vSpreadForward.z = forward.z + ( fSpreadX * right.z ) + ( fSpreadY * up.z );
				vSpreadForward.normalise( );

				sdk::vec3 qaNewAngle;
				utilities::math::vector_angles( vSpreadForward, qaNewAngle );
				qaNewAngle = utilities::math::normalize_angle( qaNewAngle );

				sdk::vec3 vEnd;
				utilities::math::angle_vectors( qaNewAngle, &vEnd );

				return ( ( vEnd - forward ) * distance ).length( );
			};
			auto GetIdealPointscale = [ this, entity, eye_position, GetSpreadDistance ]( int hitbox_index ) -> float {
				const auto hitbox = GetHitbox( entity, hitbox_index );
				if ( !hitbox )
					return 0.f;

				const float hitbox_radius = hitbox->m_flRadius,
					spread_distance = GetSpreadDistance( ( eye_position - entity->position( ) ).length( ) );

				float ideal_pointscale = 0.f;
				if ( spread_distance > hitbox_radius * 0.5f )
					return ideal_pointscale;

				ideal_pointscale = ( ( hitbox_radius * 0.5f ) - spread_distance ) / ( hitbox_radius * 0.5f );

				return utilities::clamp<float>( ideal_pointscale - 0.1f, 0.f, 0.8f );
			};
			if ( g_config.aimbot.hitscan_head_multipoint) { /// head
				CreateAndAddBufferToVec( positions, 8, GetIdealPointscale( 8 ) );
			}
			if (g_config.aimbot.hitscan_body_multipoint) { /// chest
				CreateAndAddBufferToVec( positions, 6, GetIdealPointscale( 6 ) );
				CreateAndAddBufferToVec( positions, 5, GetIdealPointscale( 5 ) );
			}
			if (g_config.aimbot.hitscan_stomach_multipoint) { /// stomach
				CreateAndAddBufferToVec( positions, 4, GetIdealPointscale( 4 ) );
				CreateAndAddBufferToVec( positions, 3, GetIdealPointscale( 3 ) );
			}
			if (g_config.aimbot.hitscan_arms_multipoint) { /// arms
				CreateAndAddBufferToVec( positions, 18, GetIdealPointscale( 18 ) );
				CreateAndAddBufferToVec( positions, 16, GetIdealPointscale( 16 ) );
			}
			if (g_config.aimbot.hitscan_legs_multipoint && !ignore_limbs ) /// legs
			{
				CreateAndAddBufferToVec( positions, 9, GetIdealPointscale( 9 ) );
				CreateAndAddBufferToVec( positions, 8, GetIdealPointscale( 8 ) );
			}
			if (g_config.aimbot.hitscan_feets_multipoint && !ignore_limbs ) { /// feet
				CreateAndAddBufferToVec( positions, 13, GetIdealPointscale( 13 ) );
				CreateAndAddBufferToVec( positions, 12, GetIdealPointscale( 12 ) );
				CreateAndAddBufferToVec( positions, 11, GetIdealPointscale( 11 ) );
				CreateAndAddBufferToVec( positions, 10, GetIdealPointscale( 10 ) );
			}

			return positions;
		};
		auto ConstructHitscanPositions = [ this, entity ]( bool ignore_limbs ) -> std::vector<std::pair<sdk::vec3, int>> {
			std::vector<std::pair<sdk::vec3, int>> positions;
			if ( g_config.aimbot.hitscan_head) { /// head
				positions.emplace_back( GetHitboxPosition( entity, 8 ), 8 );
			}
			if ( g_config.aimbot.hitscan_body ) { /// chest
				positions.emplace_back( GetHitboxPosition( entity, 6 ), 6 );
				positions.emplace_back( GetHitboxPosition( entity, 5 ), 5 );
			}
			if ( g_config.aimbot.hitscan_stomach) { /// stomach
				positions.emplace_back( GetHitboxPosition( entity, 4 ), 4 );
				positions.emplace_back( GetHitboxPosition( entity, 3 ), 3 );
			}
			if ( g_config.aimbot.hitscan_arms ) /// arms
			{
				positions.emplace_back( GetHitboxPosition( entity, 18 ), 18 );
				positions.emplace_back( GetHitboxPosition( entity, 16 ), 16 );
			}
			if ( g_config.aimbot.hitscan_legs && !ignore_limbs ) { /// legs
				positions.emplace_back( GetHitboxPosition( entity, 9 ), 9 );
				positions.emplace_back( GetHitboxPosition( entity, 8 ), 8 );
			}
			if ( g_config.aimbot.hitscan_feets && !ignore_limbs ) { /// feet
				positions.emplace_back( GetHitboxPosition( entity, 13 ), 13 );
				positions.emplace_back( GetHitboxPosition( entity, 12 ), 12 );
				positions.emplace_back( GetHitboxPosition( entity, 11 ), 11 );
				positions.emplace_back( GetHitboxPosition( entity, 10 ), 10 );
			}
			return positions;
		};

		/// get the positions
		const auto hitscan_mp_positions = ConstructHitscanMultipointPositions( should_ignore_limbs );
		const auto hitscan_positions = ConstructHitscanPositions( should_ignore_limbs );

		/// get the best multipoint hitscan position
		int mp_damage = -1, mp_min_damage = 0;
		sdk::vec3 mp_position;
		for ( const auto& pair : hitscan_mp_positions ) {
			const auto info = features::ragebot::autowall.CalculateDamage( eye_position, pair.first, local_player, entity, is_taser ? 0 : -1 );
			const int min_dmg = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;

			if ( info.damage > mp_damage ) {
				mp_damage = info.damage, mp_position = pair.first;
				mp_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
			}
		}

		/// get the best hitscan position
		int hitscan_damage = -1, hitscan_min_damage = 0, hitscan_hitbox = 0;
		sdk::vec3 hitscan_position;
		for ( const auto& pair : hitscan_positions ) {
			const auto info = features::ragebot::autowall.CalculateDamage( eye_position, pair.first, local_player, entity, is_taser ? 0 : -1 );
			const bool has_pos_already = hitscan_damage != -1;

			if ( info.damage > hitscan_damage ) {
				/// feet are the least priority
				//const bool is_feet = pair.second >= 10 && pair.second <= 13;
				//if (is_feet && has_pos_already)
				//continue;

				hitscan_hitbox = pair.second;
				hitscan_damage = info.damage, hitscan_position = pair.first;
				hitscan_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
			}
		}

		/// no positions deal enough damage
		if ( hitscan_damage < hitscan_min_damage && mp_damage < mp_min_damage )
			return false;

		/// one of these are not valid, choose the one that is valid
		if ( hitscan_damage < hitscan_min_damage || mp_damage < mp_min_damage ) {
			if ( hitscan_damage >= hitscan_min_damage ) {
				final_position = hitscan_position;
				final_damage = hitscan_damage;
				return true;
			}
			else {
				final_position = mp_position;
				final_damage = mp_damage;
				return true;
			}
		}

		/// just choose the position that does most damage gg
		auto PrioritiseHitscan = [ mp_damage, hitscan_hitbox, hitscan_damage, entity ]( ) -> bool {
			switch ( hitscan_hitbox ) {
			case 8://
			case 7://
			case 6:
			case 5:
			case 4:
			case 3:
			{
				/// is lethal
				if ( hitscan_damage > entity->health( ) + 10 )
					return true;

				if ( mp_damage > entity->health( ) )
					return false;

				/// less than 10 damage difference, prefer hitscan over multipoint 
				if ( abs( mp_damage - hitscan_damage ) < 10 )
					return true;
			}
			default:
				return false;
			}
		};
		if ( hitscan_damage > mp_damage || PrioritiseHitscan( ) ) {
			final_position = hitscan_position;
			final_damage = hitscan_damage;
		}
		else {
			final_position = mp_position;
			final_damage = mp_damage;
		}

		return true;
	}

	bool automatic_shoot_at_target_bot::DoHeadAim( sdk::base_entity* entity, sdk::vec3& final_position, int& final_damage ) {
		/// default value
		final_damage = 0;

		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player )
			return false;

		auto weapon = (sdk::base_weapon*)interfaces::entity_list->get_entity(local_player->active_weapon_handle());
		if ( !weapon )
			return false;

		//if ( UTILS::IsWeaponTaser( weapon ) )
		//	return false;

		const auto eye_position = local_player->position( ) + local_player->view_offset( );
		const float pointscale = g_config.aimbot.pointscale_head / 100;

		/// if taser or low on ammo, set minimum damage to their health so that it kills in a single shot
		const int minimum_damage = ( weapon->clip1_count( ) <= 2 ) ? entity->health( ) : utilities::minimum<int>( g_config.aimbot.min_dmg, entity->health( ) + 10 );
		const int minimum_autowall_damage = ( weapon->clip1_count( ) <= 2 ) ? entity->health( ) : utilities::minimum<int>(g_config.aimbot.min_awall_dmg, entity->health( ) + 10 );

		/// center of the hitbox
		const auto head_center_position = GetHitboxPosition( entity, 0 );
		const auto head_center_info = features::ragebot::autowall.CalculateDamage( eye_position, head_center_position, local_player, entity );
		const int head_center_damage = head_center_info.damage,
			head_center_min_damage = head_center_info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;

		/// the multipoints around the center of the hitbox
		std::vector<sdk::vec3> head_multipoints;
		GetMultipointPositions( entity, head_multipoints, 0, pointscale );

		/// get the multipoint position that does the most damage
		sdk::vec3 best_multipoint_position;
		int best_multipoint_damage = -1, multipoint_min_damage = 0;
		for ( const auto& x : head_multipoints ) {
			const auto info = features::ragebot::autowall.CalculateDamage( eye_position, x, local_player, entity );

			if ( info.damage > best_multipoint_damage ) {
				best_multipoint_damage = info.damage, best_multipoint_position = x;
				multipoint_min_damage = info.did_penetrate_wall ? minimum_autowall_damage : minimum_damage;
			}
		}

		/// none are valid
		if ( head_center_damage < minimum_damage && best_multipoint_damage < minimum_damage )
			return false;

		/// one of these aren't valid, choose the valid one
		if ( head_center_damage < head_center_min_damage || best_multipoint_damage < multipoint_min_damage ) {
			if ( head_center_damage > best_multipoint_damage ) {
				final_position = head_center_position;
				final_damage = head_center_damage;
			}
			else {
				final_position = best_multipoint_position;
				final_damage = best_multipoint_damage;
			}
		}
		else /// both are valid, do some more decision making 
		{
			/// decide whether to shoot the center of the hitbox or the multipoints of the hitbox
			if ( head_center_damage > entity->health( ) + 10 || head_center_damage + 10 > best_multipoint_damage ) {
				final_position = head_center_position;
				final_damage = head_center_damage;
			}
			else {
				final_position = best_multipoint_position;
				final_damage = best_multipoint_damage;
			}
		}

		return true;
	}

	bool automatic_shoot_at_target_bot::IsAccurate( sdk::base_entity* entity, sdk::vec3 position ) {
		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player )
			return false;

		auto weapon = (sdk::base_weapon*)interfaces::entity_list->get_entity(local_player->active_weapon_handle());
		if ( !weapon )
			return false;

		//if ( UTILS::IsWeaponKnife( weapon ) || UTILS::IsWeaponTaser( weapon ) )
			//return true;

		if ( g_config.aimbot.hitchance_enabled) {
			auto RandomFloat = [ ]( float min, float max ) -> float {
				typedef float( *RandomFloat_t )( float, float );
				static RandomFloat_t m_RandomFloat = ( RandomFloat_t )GetProcAddress( GetModuleHandle( "vstdlib.dll" ), "RandomFloat" );
				return m_RandomFloat( min, max );
			};

			sdk::vec3 angle = utilities::math::normalize_angle( utilities::math::calculate_angle( local_player->position( ) + local_player->view_offset( ), position ) );

			sdk::vec3 forward, right, up;
			utilities::math::angle_vectors( angle, &forward, &right, &up );

			int iHit = 0;
			weapon->update_accuraty_penalty( );
			for ( int i = 0; i < 256; i++ ) {
				float RandomA = RandomFloat( 0.0f, 1.0f );
				float RandomB = 1.0f - RandomA * RandomA;

				RandomA = RandomFloat( 0.0f, M_PI_F * 2.0f );
				RandomB *= weapon->spread_cone( );

				float SpreadX1 = ( cos( RandomA ) * RandomB );
				float SpreadY1 = ( sin( RandomA ) * RandomB );

				float RandomC = RandomFloat( 0.0f, 1.0f );
				float RandomF = 1.0f - RandomC * RandomC;

				RandomC = RandomFloat( 0.0f, M_PI_F * 2.0f );
				RandomF *= weapon->inaccuracy( );

				float SpreadX2 = ( cos( RandomC ) * RandomF );
				float SpreadY2 = ( sin( RandomC ) * RandomF );

				float fSpreadX = SpreadX1 + SpreadX2;
				float fSpreadY = SpreadY1 + SpreadY2;

				sdk::vec3 vSpreadForward;
				vSpreadForward.x = forward.x + ( fSpreadX * right.x ) + ( fSpreadY * up.x );
				vSpreadForward.y = forward.y + ( fSpreadX * right.y ) + ( fSpreadY * up.y );
				vSpreadForward.z = forward.z + ( fSpreadX * right.z ) + ( fSpreadY * up.z );
				vSpreadForward.normalize_in_place( );

				sdk::vec3 qaNewAngle;
				utilities::math::vector_angles( vSpreadForward, qaNewAngle );
				qaNewAngle = utilities::math::normalize_angle( qaNewAngle );

				sdk::vec3 vEnd;
				utilities::math::angle_vectors( qaNewAngle, &vEnd );
				vEnd = local_player->position( ) + local_player->view_offset( ) + ( vEnd * 8192.f );

				sdk::trace_t trace;
				sdk::trace_filter_one_entity2 filter;
				filter.pEntity = entity;
				sdk::ray_t ray;
				ray.initialize( local_player->position( ) + local_player->view_offset( ), vEnd );

				interfaces::trace->TraceRay( ray, MASK_ALL, &filter, &trace );
				if ( trace.m_pEnt == entity )
					iHit++;

				if ( iHit / 256.f >= g_config.aimbot.hitchance / 100.f )
					return true;
			}

			return false;
		} else {
			return true;
		}

		return false;
	}

	void automatic_shoot_at_target_bot::GetMultipointPositions( sdk::base_entity* entity, std::vector<sdk::vec3>& positions, int hitbox_index, float pointscale ) {
		const auto hitbox = GetHitbox( entity, hitbox_index );
		if ( !hitbox )
			return;

		const float hitbox_radius = hitbox->m_flRadius * pointscale;

		// 2 spheres
		sdk::vec3 Min = hitbox->bbmin, Max = hitbox->bbmax;

		if ( hitbox->m_flRadius == -1.f ) {
			const auto center = ( Min + Max ) * 0.5f;

			positions.emplace_back( );
		}
		else {
			// Points 0 - 5 = min
			// Points 6 - 11 = max
			sdk::vec3 P[ 12 ];
			for ( int j = 0; j < 6; j++ ) {
				P[ j ] = Min;
			}
			for ( int j = 7; j < 12; j++ ) {
				P[ j ] = Max;
			}

			P[ 1 ].x += hitbox_radius;
			P[ 2 ].x -= hitbox_radius;
			P[ 3 ].y += hitbox_radius;
			P[ 4 ].y -= hitbox_radius;
			P[ 5 ].z += hitbox_radius;

			P[ 6 ].x += hitbox_radius;
			P[ 7 ].x -= hitbox_radius;
			P[ 8 ].y += hitbox_radius;
			P[ 9 ].y -= hitbox_radius;
			P[ 10 ].z += hitbox_radius;
			P[ 11 ].z -= hitbox_radius;

			for ( int j = 0; j < 12; j++ ) {
				utilities::math::vector_transform( P[ j ], entity->get_bone_matrix( hitbox->bone ), P[ j ] );
				//Points[j] += OriginOffset;
				positions.push_back( P[ j ] );
			}
		}
	}

	sdk::vec3 automatic_shoot_at_target_bot::GetHitboxPosition( sdk::base_entity* entity, int hitbox_index ) {
		const auto hitbox = GetHitbox( entity, hitbox_index );
		if ( !hitbox )
			return sdk::vec3( 0, 0, 0 );

		const auto bone_matrix = entity->get_bone_matrix( hitbox->bone );

		sdk::vec3 bbmin, bbmax;
		utilities::math::vector_transform( hitbox->bbmin, bone_matrix, bbmin );
		utilities::math::vector_transform( hitbox->bbmax, bone_matrix, bbmax );

		return ( bbmin + bbmax ) * 0.5f;
	}

	sdk::mstudiobbox_t* automatic_shoot_at_target_bot::GetHitbox( sdk::base_entity* entity, int hitbox_index ) {
		if ( entity->health( ) || entity->health( ) <= 0 )
			return nullptr;

		const auto pModel = entity->get_model( );
		if ( !pModel )
			return nullptr;

		auto pStudioHdr = interfaces::model_information->GetStudioModel( pModel );
		if ( !pStudioHdr )
			return nullptr;

		auto pSet = pStudioHdr->GetHitboxSet( 0 );
		if ( !pSet )
			return nullptr;

		if ( hitbox_index >= pSet->numhitboxes || hitbox_index < 0 )
			return nullptr;

		return pSet->GetHitbox( hitbox_index );
	}
}
