#include "autowall.hh"

namespace features::ragebot{
	automatic_wall autowall;

	automatic_wall::Autowall_Return_Info automatic_wall::CalculateDamage( sdk::vec3 start, sdk::vec3 end, sdk::base_entity* from_entity, sdk::base_entity* to_entity, int specific_hitgroup )
	{
		// default values for return info, in case we need to return abruptly
		Autowall_Return_Info return_info;
		return_info.damage = -1;
		return_info.hitgroup = -1;
		return_info.hit_entity = nullptr;
		return_info.penetration_count = 4;
		return_info.thickness = 0.f;
		return_info.did_penetrate_wall = false;

		Autowall_Info autowall_info;
		autowall_info.penetration_count = 4;
		autowall_info.start = start;
		autowall_info.end = end;
		autowall_info.current_position = start;
		autowall_info.thickness = 0.f;

		// direction 
		utilities::math::angle_vectors( utilities::math::calculate_angle( start, end ), &autowall_info.direction );

		// attacking entity
		if ( !from_entity )
			from_entity = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !from_entity )
			return return_info;

		auto filter_player = sdk::trace_filter_one_entity( );
		filter_player.pEntity = to_entity;
		auto filter_local = sdk::trace_filter( );
		filter_local.skip = from_entity;

		// setup filters
		if ( to_entity )
			autowall_info.filter = &filter_player;
		else
			autowall_info.filter = &filter_player;

		// weapon
		auto weapon = reinterpret_cast< sdk::base_weapon* >( interfaces::entity_list->get_entity( from_entity->active_weapon_handle( ) ) );
		if ( !weapon )
			return return_info;

		// weapon data
		auto weapon_info = weapon->weapon_data( );
		if ( !weapon_info )
			return return_info;

		// client class
		auto weapon_client_class = reinterpret_cast< sdk::base_entity* >( weapon )->client_class( );
		if ( !weapon_client_class )
			return return_info;

		// weapon range
		float range = min( weapon_info->range, ( start - end ).length( ) );
		end = start + ( autowall_info.direction * range );
		autowall_info.current_damage = weapon_info->damage;

		while ( autowall_info.current_damage > 0 && autowall_info.penetration_count > 0 )
		{
			return_info.penetration_count = autowall_info.penetration_count;

			UTIL_TraceLine( autowall_info.current_position, end, MASK_SHOT | CONTENTS_GRATE, from_entity, &autowall_info.enter_trace );
			UTIL_ClipTraceToPlayers( autowall_info.current_position, autowall_info.current_position + autowall_info.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, autowall_info.filter, &autowall_info.enter_trace );

			const float distance_traced = ( autowall_info.enter_trace.end - start ).length( );
			autowall_info.current_damage *= pow( weapon_info->range_modifier, ( distance_traced / 500.f ) );

			/// if reached the end
			if ( autowall_info.enter_trace.flFraction == 1.f )
			{
				if ( to_entity && specific_hitgroup != -1 )
				{
					ScaleDamage( to_entity, weapon_info, specific_hitgroup, autowall_info.current_damage );

					return_info.damage = autowall_info.current_damage;
					return_info.hitgroup = specific_hitgroup;
					return_info.end = autowall_info.enter_trace.end;
					return_info.hit_entity = to_entity;
				}
				else
				{
					return_info.damage = autowall_info.current_damage;
					return_info.hitgroup = -1;
					return_info.end = autowall_info.enter_trace.end;
					return_info.hit_entity = nullptr;
				}

				break;
			}
			// if hit an entity
			if ( autowall_info.enter_trace.hitGroup > 0 && autowall_info.enter_trace.hitGroup <= 7 && autowall_info.enter_trace.m_pEnt )
			{
				// checkles gg
				if ( ( to_entity && autowall_info.enter_trace.m_pEnt != to_entity ) ||
					( autowall_info.enter_trace.m_pEnt->team( ) == from_entity->team( ) ) )
				{
					return_info.damage = -1;
					return return_info;
				}

				if ( specific_hitgroup != -1 )
					ScaleDamage( autowall_info.enter_trace.m_pEnt, weapon_info, specific_hitgroup, autowall_info.current_damage );
				else
					ScaleDamage( autowall_info.enter_trace.m_pEnt, weapon_info, autowall_info.enter_trace.hitGroup, autowall_info.current_damage );

				// fill the return info
				return_info.damage = autowall_info.current_damage;
				return_info.hitgroup = autowall_info.enter_trace.hitGroup;
				return_info.end = autowall_info.enter_trace.end;
				return_info.hit_entity = autowall_info.enter_trace.m_pEnt;

				break;
			}

			// break out of the loop retard
			if ( !CanPenetrate( from_entity, autowall_info, weapon_info ) )
				break;

			return_info.did_penetrate_wall = true;
		}

		return_info.penetration_count = autowall_info.penetration_count;

		return return_info;
	}

	float automatic_wall::GetThickness( sdk::vec3 start, sdk::vec3 end )
	{
		float current_thickness = 0.f;

		sdk::vec3 direction;
		utilities::math::angle_vectors( utilities::math::calculate_angle( start, end ), &direction );

		sdk::trace_world_only filter;
		sdk::trace_t enter_trace;
		sdk::trace_t exit_trace;
		sdk::ray_t ray;

		int pen = 0;
		while ( pen < 4 )
		{
			ray.initialize( start, end );

			interfaces::trace->TraceRay( ray, MASK_ALL, &filter, &enter_trace );
			if ( enter_trace.flFraction == 1.f )
				return current_thickness;

			start = enter_trace.end;
			if ( !TraceToExit( enter_trace, exit_trace, start, direction ) )
				return current_thickness + 90.f;

			start = exit_trace.end;
			current_thickness += ( start - exit_trace.end ).length( );

			pen++;
		}

		return current_thickness;
	}

	bool automatic_wall::CanPenetrate( sdk::base_entity* attacker, Autowall_Info& info, sdk::weapon_info* weapon_data )
	{
		//typedef bool(__thiscall* HandleBulletPenetrationFn)(sdk::base_entity*, float&, int&, int*, sdk::trace_t*, sdk::vec3*, float, float, float, int, int, float, int*, sdk::vec3*, float, float, float*);
		//CBaseEntity *pLocalPlayer, float *flPenetration, int *SurfaceMaterial, char *IsSolid, trace_t *ray, sdk::vec3 *vecDir, int unused, float flPenetrationModifier, float flDamageModifier, int unused2, int weaponmask, float flPenetration2, int *hitsleft, sdk::vec3 *ResultPos, int unused3, int unused4, float *damage)
		typedef bool( __thiscall* HandleBulletPenetrationFn )( sdk::base_entity*, float*, int*, char*, sdk::trace_t*, sdk::vec3*, int, float, float, int, int, float, int*, sdk::vec3*, int, int, float* );
		static HandleBulletPenetrationFn HBP = reinterpret_cast< HandleBulletPenetrationFn >( utilities::find_pattern( "client_panorama.dll",
			(PBYTE)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF8\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x83\xEC\x78\x56\x8B\x73\x34",
			"xxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ) );

		auto enter_surface_data = interfaces::physic_props->surface_data( info.enter_trace.surface.surfaceProps );
		if ( !enter_surface_data )
			return true;

		char is_solid = 0;
		int material = enter_surface_data->game.material;
		int mask = 0x1002;

		// glass and shit gg
		if ( info.enter_trace.m_pEnt && !strcmp( "CBreakableSurface",
			info.enter_trace.m_pEnt->client_class( )->name ) )
			*reinterpret_cast< byte* >( uintptr_t( info.enter_trace.m_pEnt + 0x27C ) ) = 2;

		is_autowalling = true;
		bool return_value = !HBP( attacker, &weapon_data->penetration, &material, &is_solid, &info.enter_trace, &info.direction, 0, enter_surface_data->game.flPenetrationModifier, enter_surface_data->game.flDamageModifier, 0, mask, weapon_data->penetration, &info.penetration_count, &info.current_position, 0, 0, &info.current_damage );
		is_autowalling = false;
		return return_value;
	}

	void automatic_wall::ScaleDamage( sdk::base_entity* entity, sdk::weapon_info* weapon_info, int hitgroup, float& current_damage )
	{
		//Cred. to N0xius for reversing this.
		//TODO: _xAE^; look into reversing this yourself sometime

		bool hasHeavyArmor = false;
		int armorValue = entity->armour( );

		//Fuck making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
		auto IsArmored = [ &entity, &hitgroup ]( )-> bool
		{
			sdk::base_entity* targetEntity = entity;
			switch ( hitgroup )
			{
			case 1:
				return targetEntity->helmet( );
			case 0:
			case 2:
			case 3:
			case 4:
			case 5:
				return true;
			default:
				return false;
			}
		};

		switch ( hitgroup )
		{
		case 1:
			current_damage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
			break;
		case 3:
			current_damage *= 1.25f;
			break;
		case 6:
		case 7:
			current_damage *= 0.75f;
			break;
		default:
			break;
		}

		if ( armorValue > 0 && IsArmored( ) )
		{
			float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weapon_info->armor_ratio / 2.f;

			//Damage gets modified for heavy armor users
			if ( hasHeavyArmor )
			{
				armorBonusRatio = 0.33f;
				armorRatio *= 0.5f;
				bonusValue = 0.33f;
			}

			auto NewDamage = current_damage * armorRatio;

			if ( hasHeavyArmor )
				NewDamage *= 0.85f;

			if ( ( ( current_damage - ( current_damage * armorRatio ) ) * ( bonusValue * armorBonusRatio ) ) > armorValue )
				NewDamage = current_damage - ( armorValue / armorBonusRatio );

			current_damage = NewDamage;
		}
	}

	bool automatic_wall::TraceToExit( sdk::trace_t& enterTrace, sdk::trace_t& exitTrace, sdk::vec3 startPosition, sdk::vec3 direction )
	{
		/*
		Masks used:
		MASK_SHOT_HULL					 = 0x600400B
		CONTENTS_HITBOX					 = 0x40000000
		MASK_SHOT_HULL | CONTENTS_HITBOX = 0x4600400B
		*/

		sdk::vec3 start, end;
		float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
		int firstContents = 0;

		while ( currentDistance <= maxDistance )
		{
			//Add extra distance to our ray
			currentDistance += rayExtension;

			//Multiply the direction sdk::vec3 to the distance so we go outwards, add our position to it.
			start = startPosition + direction * currentDistance;

			if ( !firstContents )
				firstContents = interfaces::trace->GetPointContents( start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr ); /*0x4600400B*/
			int pointContents = interfaces::trace->GetPointContents( start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr );

			if ( !( pointContents & MASK_SHOT_HULL ) || pointContents & CONTENTS_HITBOX && pointContents != firstContents ) /*0x600400B, *0x40000000*/
			{
				//Let's setup our end position by deducting the direction by the extra added distance
				end = start - ( direction * rayExtension );

				//Let's cast a ray from our start pos to the end pos
				UTIL_TraceLine( start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace );

				//Let's check if a hitbox is in-front of our enemy and if they are behind of a solid wall
				if ( exitTrace.startSolid && exitTrace.surface.flags & SURF_HITBOX )
				{
					UTIL_TraceLine( start, startPosition, MASK_SHOT_HULL, exitTrace.m_pEnt, &exitTrace );

					if ( exitTrace.DidHit( ) && !exitTrace.startSolid )
					{
						start = exitTrace.end;
						return true;
					}

					continue;
				}

				//Can we hit? Is the wall solid?
				if ( exitTrace.DidHit( ) && !exitTrace.startSolid )
				{
					//Is the wall a breakable? If so, let's shoot through it.
					if ( IsBreakableEntity( enterTrace.m_pEnt ) && IsBreakableEntity( exitTrace.m_pEnt ) )
						return true;

					if ( enterTrace.surface.flags & SURF_NODRAW || !( exitTrace.surface.flags & SURF_NODRAW ) && ( exitTrace.plane.normal.dot( direction ) <= 1.f ) )
					{
						float multAmount = exitTrace.flFraction * 4.f;
						start -= direction * multAmount;
						return true;
					}

					continue;
				}

				if ( !exitTrace.DidHit( ) || exitTrace.startSolid )
				{
					if ( enterTrace.DidHitNonWorldEntity( ) && IsBreakableEntity( enterTrace.m_pEnt ) )
					{
						exitTrace = enterTrace;
						exitTrace.end = start + direction;
						return true;
					}
				}
			}
		}
		return false;
	}

	bool automatic_wall::HandleBulletPenetration( sdk::weapon_info* weaponData, sdk::trace_t& enterTrace, sdk::vec3& eyePosition, sdk::vec3 direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, float& current_thickness )
	{
		//Because there's been issues regarding this- putting this here.
		if ( &currentDamage == nullptr )
			throw std::invalid_argument( "currentDamage is null!" );

		auto local_player = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );
		if ( !local_player )
			return false;

		sdk::trace_t exitTrace;
		sdk::base_entity* pEnemy = enterTrace.m_pEnt;
		sdk::surfacedata_t* enterSurfaceData = interfaces::physic_props->surface_data( enterTrace.surface.surfaceProps );
		int enterMaterial = enterSurfaceData->game.material;

		float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
		float enterDamageModifier = enterSurfaceData->game.flDamageModifier;
		float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
		bool isSolidSurf = ( ( enterTrace.contents >> 3 ) & CONTENTS_SOLID );
		bool isLightSurf = ( ( enterTrace.surface.flags >> 7 ) & SURF_LIGHT );

		if ( possibleHitsRemaining <= 0
			|| ( !possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS )
			|| weaponData->penetration <= 0.f
			|| !TraceToExit( enterTrace, exitTrace, enterTrace.end, direction )
			&& !( interfaces::trace->GetPointContents( enterTrace.end, MASK_SHOT_HULL, nullptr ) & MASK_SHOT_HULL ) )
			return false;

		sdk::surfacedata_t* exitSurfaceData = interfaces::physic_props->surface_data( exitTrace.surface.surfaceProps );
		int exitMaterial = exitSurfaceData->game.material;
		float exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
		float exitDamageModifier = exitSurfaceData->game.flDamageModifier;

		//Are we using the newer penetration system?
		if ( sv_penetration_type )
		{
			if ( enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS )
			{
				combinedPenetrationModifier = 3.f;
				finalDamageModifier = 0.05f;
			}
			else if ( isSolidSurf || isLightSurf )
			{
				combinedPenetrationModifier = 1.f;
				finalDamageModifier = 0.16f;
			}
			else if ( enterMaterial == CHAR_TEX_FLESH && ( local_player->team( ) == pEnemy->team( ) && ff_damage_reduction_bullets == 0.f ) ) //TODO: Team check config
			{
				//Look's like you aren't shooting through your teammate today
				if ( ff_damage_bullet_penetration == 0.f )
					return false;

				//Let's shoot through teammates and get kicked for teamdmg! Whatever, atleast we did damage to the enemy. I call that a win.
				combinedPenetrationModifier = ff_damage_bullet_penetration;
				finalDamageModifier = 0.16f;
			}
			else
			{
				combinedPenetrationModifier = ( enterSurfPenetrationModifier + exitSurfPenetrationModifier ) / 2.f;
				finalDamageModifier = 0.16f;
			}

			//Do our materials line up?
			if ( enterMaterial == exitMaterial )
			{
				if ( exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD )
					combinedPenetrationModifier = 3.f;
				else if ( exitMaterial == CHAR_TEX_PLASTIC )
					combinedPenetrationModifier = 2.f;
			}

			//Calculate thickness of the wall by getting the length of the range of the trace and squaring
			thickness = ( exitTrace.end - enterTrace.end ).length_sqr( );
			modifier = fmaxf( 1.f / combinedPenetrationModifier, 0.f );
			current_thickness += thickness;

			//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
			lostDamage = fmaxf(
				( ( modifier * thickness ) / 24.f )
				+ ( ( currentDamage * finalDamageModifier )
					+ ( fmaxf( 3.75f / penetrationPower, 0.f ) * 3.f * modifier ) ), 0.f );

			//Did we loose too much damage?
			if ( lostDamage > currentDamage )
				return false;

			//We can't use any of the damage that we've lost
			if ( lostDamage > 0.f )
				currentDamage -= lostDamage;

			//Do we still have enough damage to deal?
			if ( currentDamage < 1.f )
				return false;

			eyePosition = exitTrace.end;
			--possibleHitsRemaining;

			return true;
		}
		else //Legacy penetration system
		{
			combinedPenetrationModifier = 1.f;

			if ( isSolidSurf || isLightSurf )
				finalDamageModifier = 0.99f; //Good meme :^)
			else
			{
				finalDamageModifier = fminf( enterDamageModifier, exitDamageModifier );
				combinedPenetrationModifier = fminf( enterSurfPenetrationModifier, exitSurfPenetrationModifier );
			}

			if ( enterMaterial == exitMaterial && ( exitMaterial == CHAR_TEX_METAL || exitMaterial == CHAR_TEX_WOOD ) )
				combinedPenetrationModifier += combinedPenetrationModifier;

			thickness = ( exitTrace.end - enterTrace.end ).length_sqr( );

			if ( sqrt( thickness ) <= combinedPenetrationModifier * penetrationPower )
			{
				currentDamage *= finalDamageModifier;
				eyePosition = exitTrace.end;
				--possibleHitsRemaining;

				return true;
			}

			return false;
		}
	}

}
