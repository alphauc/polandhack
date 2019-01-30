#pragma once

#include "../../utilities/netvar_manager.hh"
#include "../../utilities/utilities.hh"
#include "../classes/weapon_info.hh"
#include "../interfaces/base_interface.hh"
#include "../misc/vector.hh"
#include "../misc/recv.hh"
#include "../misc/matrix.hh"
#include "../interfaces/model_info.hh"

template <class T>
static T GetVFunction( void* instance, int index ) {
	const auto vtable = *static_cast< void*** >( instance );
	return reinterpret_cast< T >( vtable[ index ] );
}

typedef sdk::client_class _client_class;
class matrix3x4_t;
namespace sdk {

	class collideable : public base_interface {
	public:
		auto mins( ) -> vec3 & { return this->get_vfunc<1, vec3 &>( this ); };
		auto maxs( ) -> vec3 & { return this->get_vfunc<2, vec3 &>( this ); };

	};

	class animation_layer {
	public:
		char pad_0000[ 20 ];
		uint32_t m_nOrder; //0x0014
		uint32_t m_nSequence; //0x0018
		float_t m_flPrevCycle; //0x001C
		float_t m_flWeight; //0x0020
		float_t m_flWeightDeltaRate; //0x0024
		float_t m_flPlaybackRate; //0x0028
		float_t m_flCycle; //0x002C
		void* m_pOwner; //0x0030 // player's thisptr
		char pad_0038[ 4 ]; //0x0034
	}; //Size: 0x0038

	class base_anim_state {
	public:
		void* pThis;
		char pad2[ 94 ];
		void* pBaseEntity; //0x60
		void* pActiveWeapon; //0x64
		void* pLastActiveWeapon; //0x68
		float m_flLastClientSideAnimationUpdateTime; //0x6C
		int m_iLastClientSideAnimationUpdateFramecount; //0x70
		float m_flEyePitch; //0x74
		float m_flEyeYaw; //0x78
		float m_flPitch; //0x7C
		float m_flGoalFeetYaw; //0x80
		float m_flCurrentFeetYaw; //0x84
		float m_flCurrentTorsoYaw; //0x88
		float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
		float m_flLeanAmount; //0x90
		char pad4[ 4 ]; //NaN
		float m_flFeetCycle; //0x98 0 to 1
		float m_flFeetYawRate; //0x9C 0 to 1
		float m_fUnknown2;
		float m_fDuckAmount; //0xA4
		float m_fLandingDuckAdditiveSomething; //0xA8
		float m_fUnknown3; //0xAC
		vec3 m_vOrigin; //0xB0, 0xB4, 0xB8
		vec3 m_vLastOrigin; //0xBC, 0xC0, 0xC4
		float m_vVelocityX; //0xC8
		float m_vVelocityY; //0xCC
		char pad5[ 4 ];
		float m_flUnknownFloat1; //0xD4 Affected by movement and direction
		char pad6[ 8 ];
		float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
		float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
		float m_unknown; //0xE8
		float speed_2d; //0xEC
		float flUpVelocity; //0xF0
		float m_flSpeedNormalized; //0xF4 //from 0 to 1
		float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
		float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
		float m_flTimeSinceStartedMoving; //0x100
		float m_flTimeSinceStoppedMoving; //0x104
		unsigned char m_bOnGround; //0x108
		unsigned char m_bInHitGroundAnimation; //0x109
		char pad7[ 10 ];
		float m_flLastOriginZ; //0x114
		float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
		float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
		char pad8[ 4 ]; //NaN
		float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
		char pad9[ 4 ]; //NaN
		float m_flUnknown3;
		char pad10[ 528 ];
	};

	class base_entity : public base_interface {
	public:
		auto get_collideable( ) -> collideable * { return this->get_vfunc<3, collideable *>( this ); };

		auto is_player( ) -> bool { return this->get_vfunc<152, bool>( this ); };
		auto is_weapon( ) -> bool { return this->get_vfunc<160, bool>( this ); };

		auto dormant( ) -> bool & {
			return *( bool * )( ( uintptr_t )this + 0xe9 );
		};
		auto health( ) -> int32_t & {
			return *( int32_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_iHealth" )>( ) );
		};
		auto alive( ) -> bool {
			return this->health( ) > 0;
		}
		auto account( ) -> int32_t & {
			return *( int32_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_iAccount" )>( ) );
		};
		// ur mother gay
		model_t* get_model( )
		{
			void* pRenderable = reinterpret_cast< void* >( uintptr_t( this ) + 0x4 );
			typedef model_t* ( __thiscall* fnGetModel )( void* );

			return GetVFunction<fnGetModel>( pRenderable, 8 )( pRenderable );
		}

		auto team( ) -> int32_t & {
			return *( int32_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBaseEntity->m_iTeamNum" )>( ) );
		};
		auto flags( ) -> int32_t & {
			return *( int32_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_fFlags" )>( ) );
		};
		auto set_flags( int flags ) -> void {
			*( int32_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_fFlags" )>( ) ) = flags;
		}
		auto armour( ) -> int32_t & {
			return *( int32_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_ArmorValue" )>( ) );
		};
		auto helmet( ) -> bool {
			return *( bool * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_bHasHelmet" )>( ) );
		}
		auto scoped( ) -> bool {
			return *( bool * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_bIsScoped" )>( ) );
		}
		auto position( ) -> vec3 & {
			return *( vec3 * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBaseEntity->m_vecOrigin" )>( ) );
		};
		auto view_offset( ) -> vec3 & {
			return *( vec3 * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_vecViewOffset[0]" )>( ) );
		};
		auto punch_angles( ) -> vec3 & {
			return *( vec3 * )( ( uintptr_t )this + 0x301C );
		};
		auto flash_duration( ) -> float* {
			return ( float* )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_flFlashDuration" )>( ) );
		}
		auto velocity( ) -> vec3 & {
			return *( vec3 * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_vecVelocity[0]" )>( ) );
		};
		auto set_velocity( vec3 velocity ) -> void {
			*( vec3 * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_vecVelocity[0]" )>( ) ) = velocity;
		}
		auto set_dormancy( vec3 velocity ) -> void {
			*( vec3 * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_vecVelocity[0]" )>( ) ) = velocity;
		}
		auto set_dormancy( bool dormant ) -> void {
			*reinterpret_cast< bool* >( reinterpret_cast< uintptr_t >( this ) + 0xE9 ) = dormant;
		}
		auto observer_mode( ) -> int32_t& {
			return *( int32_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_iObserverMode" )>( ) );
		};
		auto active_weapon_handle( ) -> uintptr_t {
			return *( uintptr_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBaseCombatCharacter->m_hActiveWeapon" )>( ) );
		}
		auto next_attack() -> float* {
			return (float*)((uintptr_t)this + netvars::get_offset_cached<fnv::hash_constexpr("CBaseCombatCharacter->m_flNextAttack")>());
		}
		auto active_weapon_index() -> uintptr_t {
			return *(uintptr_t *)((uintptr_t)this + netvars::get_offset_cached<fnv::hash_constexpr("CBaseCombatCharacter->m_hActiveWeapon")>()) & 0xFFF; //hope this is correct
		}
		auto observer_target( ) -> uintptr_t {
			return *( uintptr_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBasePlayer->m_hObserverTarget" )>( ) );
		}
		auto tick_base( ) -> int {
			return *( int * )( ( uintptr_t )this + 0x3404 ); // grr
		}
		auto networkable( ) -> void* {
			return reinterpret_cast< void* >( uintptr_t( this ) + 0x8 );
		}
		auto client_class( ) -> _client_class* {
			return this->get_vfunc<2, _client_class*>( networkable( ) );
		};
		auto abs_angles( ) -> vec3 {
			typedef vec3& ( __thiscall* Fn )( void* );
			return ( GetVFunction<Fn>( this, 11 ) )( this );
		}
		auto set_abs_angles( vec3 angles ) -> void {
			using Fn = void( __thiscall* )( base_entity*, const vec3& angles );
			static Fn func;

			if ( !func )
				func = ( Fn )( utilities::find_pattern( "client_panorama.dll", ( BYTE* )"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x64\x53\x56\x57\x8B\xF1\xE8", "xxxxxxxxxxxxxxx" ) );

			func( this, angles );
		}
		auto set_abs_origin( vec3 origin ) -> void {
			using Fn = void( __thiscall* )( base_entity*, const vec3& origin );
			static Fn func;

			if ( !func )
				func = ( Fn )( utilities::find_pattern( "client_panorama.dll", ( PBYTE )"\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1\xE8\x00\x00", "xxxxxxxxxxxxx??" ) );

			func( this, origin );
		}

		auto animstate( ) -> base_anim_state* {
			return *reinterpret_cast< base_anim_state** >( uintptr_t( this ) + 0x38A0 ); //aaaaa hardcode
		}
		auto anim_overlay( int index ) -> animation_layer {
			return ( *( animation_layer** )( ( DWORD )this + 0x2970 ) )[ index ];
		}
		auto set_anim_overlay( int index, animation_layer layer ) -> void {
			( *( animation_layer** )( ( DWORD )this + 0x2970 ) )[ index ] = layer;
		}
		auto index( ) -> int {
			return *reinterpret_cast< int* >( uintptr_t( this ) + 0x64 );
		}
		auto pose_paramter( ) -> float* {
			return ( float* )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBaseAnimating->m_flPoseParameter" )>( ) );
		}
		auto get_bone_matrix( ) -> matrix3x4_t* {
			auto offset = *reinterpret_cast< uintptr_t* >( uintptr_t( this ) + 0x2698 );
			if ( !offset )
				return nullptr;

			return reinterpret_cast< matrix3x4_t* >( offset );
		}
		auto get_bone_matrix( int bone_id ) -> matrix3x4_t {
			matrix3x4_t matrix;

			auto offset = *reinterpret_cast< uintptr_t* >( uintptr_t( this ) + 0x2698 ); //bonematrix

			if ( offset )
				matrix = *reinterpret_cast< matrix3x4_t* >( offset + 0x30 * bone_id );

			return matrix;
		}
		auto immune( ) -> bool {
			return *( bool * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_bGunGameImmunity" )>( ) );
		}
		auto update_clientside_animations( ) -> void {
			return this->get_vfunc<218, void>( this );
		}
		auto setup_bones( matrix3x4_t* bone_to_world_out, int max_bones, int bone_mask, float current_time ) -> bool {
			void* renderable = reinterpret_cast< void* >( uintptr_t( this ) + 0x4 );

			if ( !renderable )
				false;
			typedef bool( __thiscall* Fn )( void*, matrix3x4_t*, int, int, float );
			return GetVFunction<Fn>( renderable, 13 )( renderable, bone_to_world_out, max_bones, bone_mask, current_time );
		}

		auto eye_angles( ) -> vec3 {
			return *( vec3 * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_angEyeAngles" )>( ) );
		}
		auto set_eye_angles( vec3 angles ) -> void {
			*( vec3 * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_angEyeAngles" )>( ) ) = angles;
		}
		auto lower_body_yaw( ) -> float {
			return *( float * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CCSPlayer->m_flLowerBodyYawTarget" )>( ) );
		}
		auto simulation_time( ) -> float {
			return *( float* )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBaseEntity->m_flSimulationTime" )>( ) );
		}
		auto render_angles( ) -> vec3 {
			return *( vec3* )( ( DWORD )this + 0x128 );
		}
		auto set_render_angles( vec3 angles ) -> void {
			*( vec3* )( ( DWORD )this + 0x128 ) = angles;
		}
		auto eye_position( ) -> vec3 {
			return this->position( ) + this->view_offset( );
		};
		auto animating( ) -> void* {
			return reinterpret_cast< void* >( uintptr_t( this ) + 0x4 );
		}
		auto draw_model( int flags ) -> int { return this->get_vfunc<9, int, uint8_t>( animating( ), flags, 255 ); };

		auto sane( bool dormant ) -> bool {
			if ( dormant && !this->dormant( ) )
				return true;

			return this != nullptr && this->health( ) > 0 && this->is_player( );
		}
		auto has_c4( ) -> bool {
			static auto ret
				= reinterpret_cast< bool( __thiscall* )( void* ) >(
					utilities::find_occurrence( "client_panorama.dll", "56 8B F1 85 F6 74 31" )
					);

			return ret( this );
		}

	};

	class base_weapon : public base_entity {
	public:

		enum weapon_class_id {
			WEAPON_REVOLVER = 64,
			WEAPON_KNIFE = 93,
			WEAPON_KNIFE2 = 59,
			WEAPON_ZEUS = 31,
			WEAPON_G3SG1 = 11,
			WEAPON_GLOCK = 4,
			WEAPON_SCOUT = 40,
			WEAPON_AK47 = 7,
			WEAPON_AWP = 9,
			WEAPON_DEAGLE = 1,
			WEAPON_DUELIES = 2,
			WEAPON_P250 = 36,
			WEAPON_TEC9 = 30,
			WEAPON_SCAR20 = 38,
			WEAPON_NEGEV = 28,
			WEAPON_P90 = 19,
			WEAPON_PPBIZON = 26,
			WEAPON_MOLOTOV = 46,
			WEAPON_HEGRENADE = 44,
			WEAPON_SMOKE = 45,
			WEAPON_UMP = 24,
			WEAPON_MP5 = 23,
			WEAPON_MP7 = 33,
			WEAPON_AUG = 8,
			WEAPON_FIVESEVEN = 3,
			WEAPON_MAC10 = 17,
			WEAPON_FAMAS = 10,
			WEAPON_GALIL = 13,
			WEAPON_M4A4 = 16,
			WEAPON_M249 = 14,
			WEAPON_XM1014 = 25,
			WEAPON_C4 = 49,
			WEAPON_P2000 = 32
		};

		auto item_definition_index() -> int32_t & {
			return *(int32_t *)((uintptr_t)this + netvars::get_offset_cached<fnv::hash_constexpr("CBaseAttributableItem->m_iItemDefinitionIndex")>()); //hope this is right
		};
		auto next_primary_attack() -> float* {
			return (float*)((uintptr_t)this + netvars::get_offset_cached<fnv::hash_constexpr("CBaseCombatWeapon->m_flNextPrimaryAttack")>());
		}	
		auto next_secondary_attack() -> float* {
			return (float*)((uintptr_t)this + netvars::get_offset_cached<fnv::hash_constexpr("CBaseCombatWeapon->m_flNextSecondaryAttack")>());
		}
		auto post_pone_fire_ready_time() -> float* {
			return (float*)((uintptr_t)this + netvars::get_offset_cached<fnv::hash_constexpr("CWeaponCSBase->m_flPostponeFireReadyTime")>());
		}
		auto clip1_count( ) -> int32_t & {
			return *( int32_t * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBaseCombatWeapon->m_iClip1" )>( ) );
		};
		auto reloading( ) -> bool & {
			return *( bool * )( ( uintptr_t )this + netvars::get_offset_cached<fnv::hash_constexpr( "CBaseCombatWeapon->m_bInReload" )>( ) );
		};
		auto weapon_data( ) -> weapon_info* {
			return this->get_vfunc<444, weapon_info*>( this );
		};
		void update_accuraty_penalty( )
		{
			typedef void( __thiscall* Fn )( void* );
			return GetVFunction<Fn>( this, 468 )( this );
		}
		float spread_cone( )
		{
			typedef float( __thiscall* Fn )( void* );
			return GetVFunction<Fn>( this, 436 )( this );
		}
		float inaccuracy( )
		{
			typedef float( __thiscall* Fn )( void* );
			return GetVFunction<Fn>( this, 467 )( this );
		}

	};

}