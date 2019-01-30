#include "../hook_manager.hh"
#include "../../cfg.hh"
#include "../../cheat.hh"
#include "../../features/misc/misc.hh"
#include <fstream>

void __stdcall hooks::scene_end() {
	render_view->get_original<hooks::scene_end_fn>(9)(interfaces::render_view);

	static bool once = false;
	if (!once) {
		once = true;
		std::ofstream("csgo\\materials\\chams.vmt") << R"#("VertexLitGeneric"
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
			"$flat"         "1"
			"$wireframe"    "0"
			}
		)#";
	}

	static sdk::material* material = interfaces::material_system->find_material("chams", "Model textures", true, NULL);

	if (csgo::local_player) {
		for (int i = 1; i <= interfaces::global_vars->max_clients; i++) {
			sdk::base_entity* entity = (sdk::base_entity*)interfaces::entity_list->get_entity(i);

			if (i == interfaces::engine_client->get_local_player() && !g_config.players.chams.local.check)
				continue;
			if (!entity)
				continue;
			if (!entity->is_player())
				continue;
			if (entity->health() <= 0)
				continue;


			if (entity->team() != csgo::local_player->team()) {
				if (g_config.players.chams.enemy.invisible) {
					interfaces::model_render->override_material(material);
					material->set_flag((1 << 15), true);
					material->set_alpha(g_config.players.chams.enemy.alpha, 100.f);
					interfaces::render_view->modulate_colour(g_config.players.chams.enemy.invis_chams_clr);
					entity->draw_model(1);
					interfaces::model_render->override_material(nullptr);
					material->set_alpha(1.f);
				}
				if (g_config.players.chams.enemy.visible) {
					interfaces::model_render->override_material(material);
					material->set_flag((1 << 15), false);
					material->set_alpha(g_config.players.chams.enemy.alpha, 100.f);
					interfaces::render_view->modulate_colour(g_config.players.chams.enemy.vis_chams_clr);
					entity->draw_model(1);
					interfaces::model_render->override_material(nullptr);
					material->set_alpha(1.f);
				}
			}
			else {
				if (g_config.players.chams.friendly.invisible) {
					interfaces::model_render->override_material(material);
					material->set_flag((1 << 15), true);
					material->set_alpha(g_config.players.chams.friendly.alpha, 100.f);
					interfaces::render_view->modulate_colour(g_config.players.chams.friendly.invis_chams_clr);
					entity->draw_model(1);
					interfaces::model_render->override_material(nullptr);
					material->set_alpha(1.f);
				}
				if (g_config.players.chams.friendly.visible) {
					interfaces::model_render->override_material(material);
					material->set_flag((1 << 15), false);
					material->set_alpha(g_config.players.chams.friendly.alpha, 100.f);
					interfaces::render_view->modulate_colour(g_config.players.chams.friendly.vis_chams_clr);
					entity->draw_model(1);
					interfaces::model_render->override_material(nullptr);
					material->set_alpha(1.f);
				}
			}

			if (entity == csgo::local_player && g_config.players.chams.local.check) {
				interfaces::model_render->override_material(material);
				material->set_flag((1 << 15), true);
				material->set_alpha(g_config.players.chams.local.alpha, 100.f);
				interfaces::render_view->modulate_colour(g_config.players.chams.local.colour);
				entity->draw_model(1);
				interfaces::model_render->override_material(nullptr);
				material->set_alpha(1.f);
			}

			features::misc::glow_stencil( );
		}
	}
}
