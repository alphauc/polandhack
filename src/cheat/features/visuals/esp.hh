#pragma once

#include "../../../utilities/interface_manager.hh"

struct box {
	int x, y, w, h;
	box() = default;
	box(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
};

namespace features::esp {
	bool get_bounding_box(sdk::base_entity *entity, box& in);

	void draw_player_box(sdk::base_entity *player, box bbox);
	void draw_player_name(sdk::base_entity *player, box bbox);
	void draw_player_flags(sdk::base_entity *player, box bbox);
	void draw_player_health(sdk::base_entity *player, box bbox);
	void draw_player_ammo(sdk::base_entity *player, box bbox);
	void draw_player_weapon(sdk::base_entity *player, box bbox);

	void execute();

}