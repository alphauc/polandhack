#include "../../cheat.hh"
#include "esp.hh"
#include "../../../utilities/renderer.hh"
#include "../../../utilities/utilities.hh"
#include "../misc/dormancy.hh"
#include "../../cfg.hh"

static int alpha[ 65 ];
static int out_alpha[ 65 ];
int iterator;

#define color(x) sdk::colour(x.r, x.g, x.b, alpha[iterator])
#define o_color(x) sdk::colour(x.r, x.g, x.b, out_alpha[iterator])

bool features::esp::get_bounding_box( sdk::base_entity *entity, box& in ) {
	sdk::vec3 origin, min, max, flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	origin = entity->position( );
	min = entity->get_collideable( )->mins( ) + origin;
	max = entity->get_collideable( )->maxs( ) + origin;

	sdk::vec3 points[ ] = { sdk::vec3( min.x, min.y, min.z ),
		sdk::vec3( min.x, max.y, min.z ),
		sdk::vec3( max.x, max.y, min.z ),
		sdk::vec3( max.x, min.y, min.z ),
		sdk::vec3( max.x, max.y, max.z ),
		sdk::vec3( min.x, max.y, max.z ),
		sdk::vec3( min.x, min.y, max.z ),
		sdk::vec3( max.x, min.y, max.z ) };

	if ( !interfaces::debug_overlay->to_screen( points[ 3 ], flb ) || !interfaces::debug_overlay->to_screen( points[ 5 ], brt )
		|| !interfaces::debug_overlay->to_screen( points[ 0 ], blb ) || !interfaces::debug_overlay->to_screen( points[ 4 ], frt )
		|| !interfaces::debug_overlay->to_screen( points[ 2 ], frb ) || !interfaces::debug_overlay->to_screen( points[ 1 ], brb )
		|| !interfaces::debug_overlay->to_screen( points[ 6 ], blt ) || !interfaces::debug_overlay->to_screen( points[ 7 ], flt ) )
		return false;

	sdk::vec3 arr[ ] = { flb, brt, blb, frt, frb, brb, blt, flt };

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	for ( int i = 1; i < 8; i++ ) {
		if ( left > arr[ i ].x )
			left = arr[ i ].x;
		if ( bottom < arr[ i ].y )
			bottom = arr[ i ].y;
		if ( right < arr[ i ].x )
			right = arr[ i ].x;
		if ( top > arr[ i ].y )
			top = arr[ i ].y;
	}

	in.x = ( int )left;
	in.y = ( int )top;
	in.w = int( right - left );
	in.h = int( bottom - top );

	return true;

}

void features::esp::draw_player_box( sdk::base_entity *player, box bbox ) {
	if ( player->team( ) != csgo::local_player->team( ) && g_config.players.boxes.enemy ) {
		interfaces::vgui_surface->draw_rect( sdk::vec2( bbox.x, bbox.y ),
			sdk::vec2( bbox.w, bbox.h ), color( g_config.players.boxes.e_box_clr ), o_color( sdk::colour::black ) );
	}
	else if ( player->team( ) == csgo::local_player->team( ) && g_config.players.boxes.friendly ) {
		interfaces::vgui_surface->draw_rect( sdk::vec2( bbox.x, bbox.y ),
			sdk::vec2( bbox.w, bbox.h ), color( g_config.players.boxes.f_box_clr ), o_color( sdk::colour::black ) );
	}
}

void features::esp::draw_player_name( sdk::base_entity *player, box bbox ) {

	sdk::info_t info;
	interfaces::engine_client->get_player_info( iterator, &info );
	auto w = 0, h = 0;

	std::string str( info.player_name );
	interfaces::vgui_surface->get_text_size( render::fonts::visuals_general, utilities::to_wchar( info.player_name ), w, h );

	if ( player->team( ) != csgo::local_player->team( ) && g_config.players.name.enemy ) {
		interfaces::vgui_surface->draw_text( info.player_name, sdk::vec2( bbox.x + ( bbox.w / 2 ) - ( w / 2 ), bbox.y - 14 ), color( g_config.players.name.name_clr ),
			render::fonts::visuals_general );
	}
	if ( player->team( ) == csgo::local_player->team( ) && g_config.players.name.friendly ) {
		interfaces::vgui_surface->draw_text( info.player_name, sdk::vec2( bbox.x + ( bbox.w / 2 ) - ( w / 2 ), bbox.y - 14 ), color( g_config.players.name.name_clr ),
			render::fonts::visuals_general );
	}
}

void features::esp::draw_player_flags( sdk::base_entity *player, box bbox ) {
	int _y = bbox.y - 2;
	auto push_flag = [ & ] ( sdk::colour colour, std::string text ) -> void {
		interfaces::vgui_surface->draw_text( text.c_str( ), sdk::vec2( bbox.x + bbox.w + 2, _y ), color( colour ), render::fonts::visuals_flags );
		_y += 9;
	};

	sdk::base_weapon* weapon = ( sdk::base_weapon* )interfaces::entity_list->get_entity( player->active_weapon_handle( ) );

	if ( player->team( ) != csgo::local_player->team( ) ) {
		if ( g_config.players.flags.e_money ) {
			push_flag( sdk::colour( 150, 200, 80 ), std::string( "$" ).append( std::to_string( player->account( ) ) ) );
		}
		if ( g_config.players.flags.e_armour ) {
			std::string chuj;
			if ( player->helmet( ) ) chuj += "H";
			if ( player->armour( ) > 0 ) chuj += "K";
			push_flag( sdk::colour( 255, 255, 255 ), chuj );
		}
		if ( g_config.players.flags.e_zoom && player->scoped( ) ) {
			push_flag( sdk::colour( 0, 175, 255 ), "ZOOM" );
		}
		if ( g_config.players.flags.e_bomb && player->has_c4( ) ) {
			push_flag( sdk::colour( 255, 0, 0 ), "BOMB" );
		}
	}
	else if ( player->team( ) == csgo::local_player->team( ) ) {
		if ( g_config.players.flags.f_money ) {
			push_flag( sdk::colour( 150, 200, 80 ), std::string( "$" ).append( std::to_string( player->account( ) ) ) );
		}
		if ( g_config.players.flags.f_armour ) {
			std::string chuj;
			if ( player->helmet( ) ) chuj += "H";
			if ( player->armour( ) > 0 ) chuj += "K";
			push_flag( sdk::colour( 255, 255, 255 ), chuj );
		}
		if ( g_config.players.flags.f_zoom && player->scoped( ) ) {
			push_flag( sdk::colour( 0, 175, 255 ), "ZOOM" );
		}
		if ( g_config.players.flags.f_bomb && player->has_c4( ) ) {
			push_flag( sdk::colour( 255, 0, 0 ), "BOMB" );
		}
	}
}

void features::esp::draw_player_health( sdk::base_entity *player, box bbox ) {

	box temp( bbox.x - 6, bbox.y + ( bbox.h - bbox.h * ( std::clamp<int>( player->health( ), 0, 100.f ) / 100.f ) ) + 1, 2, bbox.h * ( std::clamp<int>( player->health( ), 0, 100 ) / 100.f ) - 2 );
	box temp_bg( bbox.x - 6, bbox.y + 1, 2, bbox.h - 2 );

	if ( player->team( ) != csgo::local_player->team( ) && g_config.players.health.enemy ) {
		if ( player->health( ) < 100 ) {
			interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp_bg.x - 1, temp_bg.y - 1 ), sdk::vec2( temp_bg.w + 2, temp_bg.h + 2 ), o_color( sdk::colour::black ) );
			interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp.x, temp.y ), sdk::vec2( temp.w, temp.h + 1 ), color( sdk::colour( 150, min( 255, player->health( ) * 225 / 100 ), 0 ) ) );
		}
		else {
			interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp_bg.x - 1, temp_bg.y - 1 ), sdk::vec2( temp_bg.w + 2, temp_bg.h + 2 ), o_color( sdk::colour::black ) );
			interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp.x, temp.y ), sdk::vec2( temp.w, temp.h ), color( sdk::colour::green ) );
		}
		interfaces::vgui_surface->draw_text( std::to_string( std::clamp<int>( player->health( ), 0, 100 ) ).c_str( ), sdk::vec2( temp.x - 3, temp.y ), color( sdk::colour::white ),
			render::fonts::visuals_flags );
	}
	if ( player->team( ) == csgo::local_player->team( ) && g_config.players.health.friendly ) {
		if ( player->health( ) < 100 ) {
			interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp_bg.x - 1, temp_bg.y - 1 ), sdk::vec2( temp_bg.w + 2, temp_bg.h + 2 ), o_color( sdk::colour::black ) );
			interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp.x, temp.y ), sdk::vec2( temp.w, temp.h + 1 ), color( sdk::colour( 150, min( 255, player->health( ) * 225 / 100 ), 0 ) ) );
		}
		else {
			interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp_bg.x - 1, temp_bg.y - 1 ), sdk::vec2( temp_bg.w + 2, temp_bg.h + 2 ), o_color( sdk::colour::black ) );
			interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp.x, temp.y ), sdk::vec2( temp.w, temp.h ), color( sdk::colour::green ) );
		}
		interfaces::vgui_surface->draw_text( std::to_string( std::clamp<int>( player->health( ), 0, 100 ) ).c_str( ), sdk::vec2( temp.x - 3, temp.y ), color( sdk::colour::white ),
			render::fonts::visuals_flags );
	}
}

void features::esp::draw_player_ammo( sdk::base_entity *player, box bbox ) {
	sdk::base_weapon* entity_weapon = ( sdk::base_weapon* )interfaces::entity_list->get_entity( player->active_weapon_handle( ) );

	if ( !entity_weapon )
		return;

	int clip = entity_weapon->clip1_count( );
	int max_clip = entity_weapon->weapon_data( )->max_clip;

	box temp( bbox.x + 1, bbox.y + bbox.h + 3, ( ( bbox.w * clip ) / max_clip ) - 2, 2 );
	box temp_bg( bbox.x + 1, bbox.y + bbox.h + 3, bbox.w - 2, 2 );

	if ( player->team( ) != csgo::local_player->team( ) && g_config.players.weapon.ammo ) {
		interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp_bg.x - 1, temp_bg.y - 1 ), sdk::vec2( temp_bg.w + 2, temp_bg.h + 2 ), o_color( sdk::colour::black ) );
		interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp.x, temp.y ), sdk::vec2( temp.w, temp.h ), color( g_config.players.weapon.ammo_clr ) );
	}
	if ( player->team( ) == csgo::local_player->team( ) && g_config.players.weapon.ammo && g_config.players.weapon.friendly ) {
		interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp_bg.x - 1, temp_bg.y - 1 ), sdk::vec2( temp_bg.w + 2, temp_bg.h + 2 ), o_color( sdk::colour::black ) );
		interfaces::vgui_surface->draw_filled_rect( sdk::vec2( temp.x, temp.y ), sdk::vec2( temp.w, temp.h ), color( g_config.players.weapon.ammo_clr ) );
	}
}

void features::esp::draw_player_weapon( sdk::base_entity *player, box bbox ) {
	sdk::base_weapon* weapon = ( sdk::base_weapon* )interfaces::entity_list->get_entity( player->active_weapon_handle( ) );

	if ( !weapon )
		return;

	auto w = 0, h = 0;
	auto _y = bbox.y + bbox.h + 2;

	if ( g_config.players.weapon.ammo )
		_y += 4;

	std::string str( weapon->weapon_data( )->weapon_name );
	str.erase( str.find( "weapon_" ), 7 ); // heh
	std::transform( str.begin( ), str.end( ), str.begin( ), ::toupper );

	interfaces::vgui_surface->get_text_size( render::fonts::visuals_flags, utilities::to_wchar( str.c_str( ) ), w, h );

	if ( player->team( ) != csgo::local_player->team( ) && g_config.players.weapon.enemy ) {
		interfaces::vgui_surface->draw_text( str.c_str( ), sdk::vec2( bbox.x + ( bbox.w / 2 ) - ( w / 2 ), _y ), color( sdk::colour::white ),
			render::fonts::visuals_flags );
	}
	if ( player->team( ) == csgo::local_player->team( ) && g_config.players.weapon.friendly ) {
		interfaces::vgui_surface->draw_text( str.c_str( ), sdk::vec2( bbox.x + ( bbox.w / 2 ) - ( w / 2 ), _y ), color( sdk::colour::white ),
			render::fonts::visuals_flags );
	}
}

void features::esp::execute( ) {
	if ( !csgo::local_player )
		return;

	for ( int32_t i = 0; i < interfaces::entity_list->get_entity_count( ); i++ ) {
		sdk::base_entity *entity = interfaces::entity_list->get_entity( i );
		iterator = i;
		box bbox;

		if ( !entity->sane( false ) )
			continue;

		if ( !get_bounding_box( entity, bbox ) )
			continue;

		if ( entity == csgo::local_player )
			continue;

		if ( g_config.players.dormant ) {
			features::misc::dormancy::sound_dormancy( );
		}

		float fade_ratio = 1 / 0.70f;
		const float cur_ratio = fade_ratio * interfaces::global_vars->frame_time;
		const int cur_inc = ( int )( cur_ratio * 255 );

		{ // general alpha
			int new_alpha = alpha[ i ];
			new_alpha += entity->dormant( ) ? -cur_inc : cur_inc;

			new_alpha = std::clamp< int >( new_alpha, g_config.players.dormant ? 45 : 0, 175 );

			alpha[ i ] = new_alpha;
		}
		{ // outline alpha
			int new_alpha = out_alpha[ i ];
			new_alpha += entity->dormant( ) ? -cur_inc : cur_inc;

			new_alpha = std::clamp< int >( new_alpha, g_config.players.dormant ? 45 : 0, 100 );

			out_alpha[ i ] = new_alpha;
		}

		draw_player_box( entity, bbox );
		draw_player_name( entity, bbox );
		draw_player_flags( entity, bbox );
		draw_player_health( entity, bbox );
		draw_player_ammo( entity, bbox );
		draw_player_weapon( entity, bbox );
	}
}