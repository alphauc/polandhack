#pragma once
#include "../../../cheat.hh"
#include "../../../../utilities/interface_manager.hh"
#include "../../../../utilities/math.hh"
#include "../../../../sdk/interfaces/game_events.hh"

namespace features::ragebot {
	class automatic_wall
	{
	public:
		struct Autowall_Return_Info
		{
			int damage;
			int hitgroup;
			int penetration_count;
			bool did_penetrate_wall;
			float thickness;
			sdk::vec3 end;
			sdk::base_entity* hit_entity;
		};

		Autowall_Return_Info CalculateDamage( sdk::vec3 start, sdk::vec3 end, sdk::base_entity* from_entity = nullptr, sdk::base_entity* to_entity = nullptr, int specific_hitgroup = -1 );
		float GetThickness( sdk::vec3 start, sdk::vec3 end );

		inline bool IsAutowalling( ) const
		{
			return is_autowalling;
		}

	private:
		bool is_autowalling = false;

		struct Autowall_Info
		{
			sdk::vec3 start;
			sdk::vec3 end;
			sdk::vec3 current_position;
			sdk::vec3 direction;

			sdk::i_trace_filter* filter;
			sdk::trace_t enter_trace;

			float thickness;
			float current_damage;
			int penetration_count;
		};

		bool CanPenetrate( sdk::base_entity* attacker, Autowall_Info& info, sdk::weapon_info* weapon_data );

		void ScaleDamage( sdk::base_entity* entity, sdk::weapon_info* weapon_info, int hitgroup, float& current_damage );

		void UTIL_TraceLine( sdk::vec3& absStart, sdk::vec3& absEnd, unsigned int mask, sdk::base_entity* ignore, sdk::trace_t* ptr )
		{
			sdk::ray_t ray;
			ray.initialize( absStart, absEnd );
			sdk::trace_filter filter;
			filter.skip = ignore;

			interfaces::trace->TraceRay( ray, mask, &filter, ptr );
		}

		void UTIL_ClipTraceToPlayers( const sdk::vec3& vecAbsStart, const sdk::vec3& vecAbsEnd, unsigned int mask, sdk::i_trace_filter* filter, sdk::trace_t* tr )
		{
			static DWORD dwAddress = utilities::find_pattern( "client_panorama.dll", ( BYTE* )"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x10", "xxxxxxxxxxxxxxxxxxxxxxxx????xxx" );
			if ( !dwAddress )
				return;

			_asm
			{
				MOV EAX, filter
				LEA ECX, tr
				PUSH ECX
				PUSH EAX
				PUSH mask
				LEA EDX, vecAbsEnd
				LEA ECX, vecAbsStart
				CALL dwAddress
				ADD ESP, 0xC
			}
		}

		void GetBulletTypeParameters( float& maxRange, float& maxDistance )
		{
			maxRange = 35.0;
			maxDistance = 3000.0;
		}

		bool IsBreakableEntity( sdk::base_entity* entity )
		{
			if ( !entity || !entity->index( ) )
				return false;

			//m_takeDamage isn't properly set when using the signature.
			//Back it up, set it to true, then restore.
			int takeDamageBackup = *reinterpret_cast< byte* >( uintptr_t( entity ) + 0x27C );

			auto pClass = entity->client_class( );
			if ( !pClass )
				return false;

			//				 '       ''     '      '   '
			//			    01234567890123456     012345678
			//Check against CBreakableSurface and CBaseDoor

			//Windows etc. are CBrekableSurface
			//Large garage door in Office is CBaseDoor and it get's reported as a breakable when it is not one
			//This is seperate from "CPropDoorRotating", which is a normal door.
			//Normally you would also check for "CFuncBrush" but it was acting oddly so I removed it. It's below if interested.
			//((clientClass->m_pNetworkName[1]) != 'F' || (clientClass->m_pNetworkName[4]) != 'c' || (clientClass->m_pNetworkName[5]) != 'B' || (clientClass->m_pNetworkName[9]) != 'h')

			if ( ( pClass->name[ 1 ] == 'B' && pClass->name[ 9 ] == 'e' && pClass->name[ 10 ] == 'S' && pClass->name[ 16 ] == 'e' )
				|| ( pClass->name[ 1 ] != 'B' || pClass->name[ 5 ] != 'D' ) )
				*reinterpret_cast< byte* >( uintptr_t( entity ) + 0x27C ) = 2;

			typedef bool( __thiscall* IsBreakbaleEntity_Fn )( sdk::base_entity* );
			IsBreakbaleEntity_Fn is_breakable_entity = ( IsBreakbaleEntity_Fn )utilities::find_pattern( "client_panorama.dll", ( PBYTE )"\x55\x8B\xEC\x51\x56\x8B\xF1\x85\xF6\x74\x68", "xxxxxxxxxxx" );

			bool breakable = is_breakable_entity( entity );
			*reinterpret_cast< byte* >( uintptr_t( entity ) + 0x27C ) = takeDamageBackup;

			return breakable;
		}

		bool TraceToExit( sdk::trace_t& enterTrace, sdk::trace_t& exitTrace, sdk::vec3 startPosition, sdk::vec3 direction );
		bool HandleBulletPenetration( sdk::weapon_info* weaponData, sdk::trace_t& enterTrace, sdk::vec3& eyePosition, sdk::vec3 direction, int& possibleHitsRemaining,
			float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration,
			float& current_thickness );
	};

	extern automatic_wall autowall;
}
