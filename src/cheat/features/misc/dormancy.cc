#include "dormancy.hh"
#include "misc.hh"

#include "../../../utilities/input.hh"
#include "../../../utilities/renderer.hh"
#include "../../../utilities/utilities.hh"
#include "../../../utilities/math.hh"
#include "../../cfg.hh"
#include <iostream>
#include <fstream>

#define range( x, a, b )    ( x >= a && x <= b )

void features::misc::dormancy::position_correction( int stage ) {
	for ( int i = 1; i <= interfaces::global_vars->max_clients; i++ ) {
		auto entity = interfaces::entity_list->get_entity( i );

		if ( entity == csgo::local_player )
			continue;

		if ( !viable( entity ) )
			continue;

		if ( !entity->dormant( ) && !players[ i ].set )
			players[ i ].oldpos = entity->position( );

		if ( !entity->dormant( ) || players[ i ].set )
			continue;

		entity->set_abs_origin( players[ i ].oldpos );
	}
}

bool features::misc::dormancy::set( const int index ) {
	if ( !range( index, 1, 64 ) )
		return false;

	return players[ index ].set;
}

bool features::misc::dormancy::viable( sdk::base_entity* entity ) {
	if ( !entity )
		return false;

	if ( !csgo::local_player )
		return false;

	if ( !entity->alive( ) || entity->flags( ) & ( 1 << 6 ) )
		return false;

	if ( entity->team( ) == csgo::local_player->team( ) )
		return false;

	if ( entity->client_class( )->class_id != 35 )
		return false;

	if ( entity->immune( ) )
		return false;

	return true;
}

void features::misc::dormancy::sound_dormancy( ) {
	sound_list.remove_all( );
	interfaces::engine_sound->get_active_sounds( sound_list );

	if ( !sound_list.count( ) )
		return;

	for ( int i = 0; i < sound_list.count( ); i++ ) {
		sdk::sound_info& sound = sound_list[ i ];
		if ( !range( sound.sound_source, 1, 64 ) )
			continue;

		if ( sound.sound_source == interfaces::engine_client->get_local_player( ) )
			continue;

		auto entity = reinterpret_cast< sdk::base_entity* >( interfaces::entity_list->get_entity( sound.sound_source ) );

		if ( !entity || sound.origin->is_zero( ) || !valid_sound( sound ) )
			continue;

		auto& player = players[ sound.sound_source ];
		player.entity = entity;


		sdk::vec3 src_3d, dst_3d;
		sdk::trace_t tr;
		sdk::ray_t ray;
		sdk::trace_filter filter;

		filter.skip = entity;
		src_3d = ( *sound.origin ) + sdk::vec3( 0, 0, 1 );
		dst_3d = src_3d - sdk::vec3( 0, 0, 100 );
		ray.initialize( src_3d, dst_3d );

		interfaces::trace->TraceRay( ray, MASK_PLAYERSOLID, &filter, &tr );

		if ( tr.allsolid ) // if stuck
			player.sounddata.time = -1;

		*sound.origin = ( ( tr.flFraction < 0.97 ) ? tr.end : *sound.origin );
		player.flags = entity->flags( );
		player.flags |= ( tr.flFraction < 0.50f ? ( 1 << 1 ) /*ducking*/ : 0 ) | ( tr.flFraction != 1 ? ( 1 << 0 ) /*on ground*/ : 0 );
		player.flags &= ( tr.flFraction > 0.50f ? ~( 1 << 1 ) /*ducking*/ : 0 ) | ( tr.flFraction == 1 ? ~( 1 << 0 ) /*on ground*/ : 0 );

		player.oldpos = *sound.origin;
		player.sounddata.time = GetTickCount( );

		if ( !entity->dormant( ) )
			continue;

		adjust_player( sound.sound_source );
	}

	soundbuffer = sound_list;
}

void features::misc::dormancy::adjust_player( const int index ) {
	if ( !g_config.players.dormant )
		return;

	auto& player = players[ index ];

	static int duration = 1000;
	bool expired = GetTickCount( ) - player.sounddata.time > duration;

	if ( expired )
		player.set = false;

	if ( !expired )
		player.set = true;

	// dormant check
	player.entity->flags( ) = player.flags;
	player.entity->position( ) = player.oldpos;
	player.entity->set_abs_origin( player.oldpos );
}

void features::misc::dormancy::restore_sound_dormancy( ) {
	for ( auto& player : players ) {
		if ( player.set )
			player.set = false;
	}
}

bool features::misc::dormancy::valid_sound( sdk::sound_info & sound ) {
	if ( !sound.from_server )
		return false;

	for ( int i = 0; i < soundbuffer.count( ); i++ ) {
		sdk::sound_info& cached_sound = soundbuffer[ i ];
		if ( cached_sound.guid == sound.guid )
			return false;
	}

	return true;
}
