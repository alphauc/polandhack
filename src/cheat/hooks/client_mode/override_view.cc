#include "../hook_manager.hh"
#include "../../cheat.hh"
#include "../../cfg.hh"

void __stdcall hooks::override_view(sdk::view_setup* viewsetup) {
	auto original_fn = client_mode->get_original<hooks::override_view_fn>(18);

	original_fn(viewsetup);

	if (interfaces::engine_client->is_connected() && interfaces::engine_client->is_in_game()) {
		if (csgo::local_player) {
			if (!csgo::local_player->scoped())
				viewsetup->fov = g_config.visuals.fov;

			if (g_config.visuals.override_fov_scoped && csgo::local_player->scoped())
				viewsetup->fov = g_config.visuals.fov;
		}
	}
}