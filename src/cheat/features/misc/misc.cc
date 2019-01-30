#define _CRT_SECURE_NO_DEPRECATE

#include "misc.hh"

#include "../../../utilities/interface_manager.hh"
#include "../../../utilities/input.hh"
#include "../../../utilities/renderer.hh"
#include "../../../utilities/utilities.hh"
#include "../../../utilities/math.hh"
#include "../../cfg.hh"
#include <iostream>
#include <fstream>

#define degrees_2_radians( x ) ( ( float )( x ) * ( float )( ( float )( 3.14159265358979323846f ) / 180.0f ) )

class c_key_values {
public:
	char _pad[ 0x20 ];
};

void features::misc::spectators( ) {
	if ( !g_config.visuals.spectators )
		return;

	if ( interfaces::engine_client->is_in_game( ) && interfaces::engine_client->is_connected( ) ) {
		int index = 0;
		int w, h;
		interfaces::engine_client->get_screen_size( w, h );

		interfaces::vgui_surface->draw_text( "spectators", sdk::vec2( w - 80, h / 2 - 10 ), sdk::colour::white, render::fonts::misc_specs );
		for ( int i = 1; i < 65; i++ ) {
			auto p_local = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) ); // what's csgo::local_player???????
			auto p_entity = interfaces::entity_list->get_entity( i );
			sdk::info_t e_info;

			if ( p_entity && p_entity != p_local ) {
				interfaces::engine_client->get_player_info( i, &e_info );
				if ( !p_entity->alive( ) && !p_entity->dormant( ) ) {
					auto target = p_entity->observer_target( );
					if ( target ) {
						auto p_target = interfaces::entity_list->get_entity( target );
						if ( p_target == p_local ) {
							int x, y;
							interfaces::vgui_surface->get_text_size( render::fonts::misc_specs, utilities::to_wchar( e_info.player_name ), x, y );

							std::string player_name = e_info.player_name;
							sdk::info_t p_info;
							interfaces::engine_client->get_player_info( i, &e_info );

							interfaces::vgui_surface->draw_text( player_name.c_str( ), sdk::vec2( w - x - 35, h / 2 + ( 10 * index ) ), sdk::colour::white, render::fonts::misc_specs );
							index++;
						}
					}
				}
			}
		}
	}
}

void features::misc::offscreen_esp( ) {
	if ( !g_config.players.pov_arrows )
		return;

	for ( int i = 1; i < 65; i++ ) {
		sdk::base_entity* entity = interfaces::entity_list->get_entity( i ); // get the valid entity

		if ( !entity )
			continue;

		sdk::vec3 viewangles = interfaces::engine_client->get_view_angles( );
		int w, h;
		interfaces::engine_client->get_screen_size( w, h );
		const auto radius = 400;
		const auto screen_center = sdk::vec2( w / 2.f, h / 2.f );
		float rotation = degrees_2_radians( viewangles.y - utilities::math::calculate_angle( csgo::local_player->position( ), entity->position( ) ).y - 90 );

		interfaces::vgui_surface->set_colour( g_config.players.pov_arrows_clr );

		sdk::vertex_t verts[ 3 ];
		std::vector<sdk::vertex_t> vertices;
		verts[ 0 ].position.x = screen_center.x + cosf( rotation ) * radius;
		verts[ 0 ].position.y = screen_center.y + sinf( rotation ) * radius;
		verts[ 1 ].position.x = screen_center.x + cosf( rotation + degrees_2_radians( 2 ) ) * ( radius - 20 );
		verts[ 1 ].position.y = screen_center.y + sinf( rotation + degrees_2_radians( 2 ) ) * ( radius - 20 );
		verts[ 2 ].position.x = screen_center.x + cosf( rotation - degrees_2_radians( 2 ) ) * ( radius - 20 );
		verts[ 2 ].position.y = screen_center.y + sinf( rotation - degrees_2_radians( 2 ) ) * ( radius - 20 );


		interfaces::vgui_surface->polygon( 3, verts, true );

	}
}

void features::misc::auto_strafer(sdk::user_cmd* command) {
	if (!g_config.movement.auto_strafer)
		return;
}

void features::misc::world_modulation() { // not final (add fullbright here) //this might affect performance in the current state //TODO: make this work with map changes... :D

	for (sdk::material_handle_t i = interfaces::material_system->first_material(); i != interfaces::material_system->invalid_material_handle(); i = interfaces::material_system->next_material(i)) {

		sdk::material* p_material = interfaces::material_system->get_material(i);

		if (!p_material)
			continue;

		if (strstr(p_material->get_group_name(), "World") || strstr(p_material->get_group_name(), "StaticProp")) {

			if (g_config.visuals.world.night) {
				p_material->set_color(0.16f, 0.16f, 0.16f);

			}
			else {
				p_material->set_color(1.f, 1.f, 1.f);
			}
		}
	}
}

void features::misc::remove_smoke() {
	if (!g_config.visuals.remove_smoke)
		return;

	static auto smoke_count = *reinterpret_cast<uint32_t **>(utilities::find_occurrence("client_panorama.dll", "A3 ? ? ? ? 57 8B CB") + 1);

	static std::vector<const char*> smoke_materials = {
		"particle/vistasmokev1/vistasmokev1_fire",
		"particle/vistasmokev1/vistasmokev1_smokegrenade",
		"particle/vistasmokev1/vistasmokev1_emods",
		"particle/vistasmokev1/vistasmokev1_emods_impactdust"
	};

	for (auto material_name : smoke_materials) {
		sdk::material * material = interfaces::material_system->find_material(material_name, TEXTURE_GROUP_OTHER);

		//material->set_flag(sdk::MATERIAL_VAR_WIREFRAME, true);
		material->set_flag(sdk::MATERIAL_VAR_NO_DRAW, true);
	}
	*(int*)smoke_count = 0;
}

void features::misc::remove_flash() {
	if (!g_config.visuals.remove_flash)
		return;

	if (!interfaces::engine_client->is_connected() && !interfaces::engine_client->is_in_game())
		return;

	auto p_local = interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());

	if (*p_local->flash_duration() > 0)
		*p_local->flash_duration() = 0;
}

void features::misc::scope_lines() {
	if (!g_config.visuals.remove_scope)
		return;

	if (!interfaces::engine_client->is_connected() && !interfaces::engine_client->is_in_game())
		return;

	auto p_local = interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());

	if (!p_local->scoped())
		return;

	int w, h;
	interfaces::engine_client->get_screen_size(w, h);

	interfaces::vgui_surface->set_colour(sdk::colour::black);
	interfaces::vgui_surface->line(0, h / 2, w, h / 2);
	interfaces::vgui_surface->line(w / 2, 0, w / 2, h);
}

void features::misc::glow_bloom( ) {
	for ( int i = 0; i < interfaces::glow_object_manager->size; ++i ) {

		sdk::glow_object_manager::glow_object *glow = &interfaces::glow_object_manager->definitions[ i ];

		auto entity = glow->get_entity( );

		if ( !entity )
			continue;

		if ( !entity->is_player( ) )
			continue;

		if ( !glow )
			continue;

		if ( g_config.players.glow.style != 0 )
			continue;

		if ( strstr( entity->client_class( )->name, ( "CCSPlayer" ) ) ) {
			if ( csgo::local_player && entity->team( ) != csgo::local_player->team( ) && g_config.players.glow.enemy ) {
				glow->draw( g_config.players.glow.e_glow_clr );
			}
			else if ( csgo::local_player && entity->team( ) == csgo::local_player->team( ) && g_config.players.glow.friendly ) {
				glow->draw( g_config.players.glow.f_glow_clr );
			}
		}
		glow->glow_alpha = g_config.players.glow.alpha / 50.f;
		glow->full_bloom = false;
		glow->render_when_occluded = true;
		glow->render_when_unoccluded = false;
		glow->m_nFullBloomStencilTestValue = 2;
	}

}

void features::misc::glow_stencil( ) {
	if ( g_config.players.glow.style != 1 )
		return;

	static bool once = false;
	if ( !once ) {
		once = true;
		std::ofstream( "csgo\\materials\\stencil.vmt" ) << R"#("VertexLitGeneric"
		{
			"$basetexture" "vgui/white_additive"
			"$ignorez"      "0"
			"$envmap" ""
			"$nofog"        "1"
			"$model"        "1"
			"$nocull"       "0"
			"$selfillum"    "1"
			"$halflambert"  "1"
			"$znearer"      "0"
			"$wireframe"    "1"
			}
		)#";
	}


	if ( g_config.players.glow.style == 1 ) {
		static sdk::material* wireframe_material = interfaces::material_system->find_material( "stencil", "Model textures", true, NULL );

		auto local = interfaces::entity_list->get_entity( interfaces::engine_client->get_local_player( ) );

		if ( interfaces::engine_client->is_in_game( ) && interfaces::engine_client->is_connected( ) ) {

			if ( !local )
				return;

			auto context = interfaces::material_system->get_render_context( );

			if ( !context )
				return;

			sdk::stencil_state stencil_state;
			stencil_state.enable = true;
			stencil_state.ref = 1;
			stencil_state.pass = 3;
			stencil_state.fail = 1;
			stencil_state.zfail = 3;
			stencil_state.compare = 8;
			stencil_state.mask1 = 0xFFFFFFFF;
			stencil_state.mask2 = 0xFFFFFFFF;
			context->set_stencil_state( stencil_state );
			interfaces::render_view->blend( 0 );

			for ( int i = 1; i < 65; ++i ) {

				auto entity = interfaces::entity_list->get_entity( i );

				if ( !entity )
					continue;

				if ( !entity->alive( ) )
					continue;

				if ( entity->dormant( ) )
					continue;

				entity->draw_model( 0x1 );
			}

			interfaces::render_view->blend( 1 );
			stencil_state.compare = 6;
			context->set_stencil_state( stencil_state );
			interfaces::model_render->override_material( wireframe_material );

			for ( int i = 0; i < interfaces::glow_object_manager->size; ++i ) {

				sdk::glow_object_manager::glow_object *glow = &interfaces::glow_object_manager->definitions[ i ];

				glow->m_nFullBloomStencilTestValue = 2;
				glow->m_flBloomAmount = 2.f;
			}

			wireframe_material->set_flag( ( 1 << 15 ), true );

			for ( int i = 1; i < 65; ++i ) {

				auto entity = interfaces::entity_list->get_entity( i );

				if ( !entity )
					continue;
				if ( !local )
					continue;
				if ( entity->dormant( ) )
					continue;
				if ( !entity->alive( ) )
					continue;

				if ( g_config.players.glow.enemy && entity->team( ) != local->team( ) ) {
					interfaces::render_view->modulate_colour( g_config.players.glow.e_glow_clr );
					entity->draw_model( 0x1 );
				}
				if ( g_config.players.glow.friendly && entity->team( ) == local->team( ) ) {
					interfaces::render_view->modulate_colour( g_config.players.glow.f_glow_clr );
					entity->draw_model( 0x1 );
				}
			}

			stencil_state.enable = false;
			context->set_stencil_state( stencil_state );
			interfaces::model_render->override_material( nullptr );
		}
	}

}

void features::misc::thirdperson( ) {
	static bool setting, holding, in_thirdperson, last_dead;

	if (!interfaces::engine_client->is_connected() && !interfaces::engine_client->is_in_game())
		return;

	if ( g_input.key_down( 0x56 ) && !holding ) {	
		setting = !setting;

		if ( csgo::local_player->alive( ) ) {
			interfaces::engine_client->execute_cmd( setting ? "thirdperson" : "firstperson" );
			in_thirdperson = setting;
		}

		holding = true;
	}
	else if ( !g_input.key_down(0x56) && holding )
		holding = false;

	// when dead
	if ( !csgo::local_player->alive( ) )
		return;

	if ( in_thirdperson ) {
		interfaces::engine_client->execute_cmd( "firstperson" );
		csgo::local_player->observer_mode( ) = 4; // in eye
		in_thirdperson = false;
	}

	if ( !setting && last_dead ) {
		csgo::local_player->observer_mode( ) = 4; // in eye
		last_dead = false;
	}

	if ( setting && !last_dead ) {
		csgo::local_player->observer_mode( ) = 5; // chase
		last_dead = true;
	}

}
