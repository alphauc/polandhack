#pragma once
#include "../sdk/misc/colour.hh"
#include <d3dx9.h>
#include <algorithm>
#include <map>
#include <shlobj.h>
#include <string>


struct cfg {

	struct m_exploits {
		bool god_mode;
	} exploits;

	struct m_menu {
		int m_alpha;
		bool m_open;
		int m_count;
		int m_selected;
		bool m_initialised;
		bool rainbow;
		sdk::colour m_accent = sdk::colour(180, 0, 40);
		sdk::colour m_gradient;
		int ticks;
	} menu;

	struct m_aimbot {
		bool enabled;
		bool hitchance_enabled;

		bool baim_if_unplugged;
		bool baim_always;
		bool baim_if_air;
		bool baim_if_x_damage_enabled;

		bool hitscan_head;
		bool hitscan_body;
		bool hitscan_stomach;
		bool hitscan_arms;
		bool hitscan_legs;
		bool hitscan_feets;

		bool hitscan_head_multipoint;
		bool hitscan_body_multipoint;
		bool hitscan_stomach_multipoint;
		bool hitscan_arms_multipoint;
		bool hitscan_legs_multipoint;
		bool hitscan_feets_multipoint;

		int pointscale_head;
		int pointscale;

		int hitchance;

		int baim_if_x_damage;

		int min_dmg;
		int min_awall_dmg;

		float delay_shot;
	} aimbot;

	struct m_anti_aim {
		struct {
			int yaw;
		} stand;

		struct {
			int yaw;
		} move;

		struct {
			int yaw;
		} air;

		int fake_yaw;
		int pitch;
		bool fake_body;
		bool enable;
		bool edge;
		int base;

		bool fakelag_check_standing;
		bool fakelag_check_moving;
		bool fakelag_check_air;
		int fakelag_type;
		int fakelag_packets;
	} anti_aim;

	struct m_players {
		bool exploit;
		float speed;
		bool dormant;
		bool pov_arrows;

		sdk::colour weapon_clr;
		sdk::colour pov_arrows_clr;

		struct {
			bool e_money;
			bool e_armour;
			bool e_zoom;
			bool e_flashed;
			bool e_reloading;
			bool e_bomb;

			bool f_money;
			bool f_armour;
			bool f_zoom;
			bool f_flashed;
			bool f_reloading;
			bool f_bomb;

		} flags;

		struct {
			bool friendly;
			bool enemy;

			sdk::colour f_box_clr = sdk::colour(250, 200, 0);
			sdk::colour e_box_clr = sdk::colour(150, 200, 60);
		} boxes;

		struct {
			bool friendly;
			bool enemy;

			sdk::colour name_clr;
		} name;

		struct {
			bool friendly;
			bool enemy;
			bool ammo;

			sdk::colour ammo_clr = sdk::colour(180, 10, 40);
		} weapon;

		struct {
			bool friendly;
			bool enemy;
		} health;

		struct {
			bool friendly;
			bool enemy;

			sdk::colour f_skel_clr;
			sdk::colour e_skel_clr;
		} skeleton;

		struct {
			bool friendly;
			bool enemy;
			int alpha = 75;
			int style;

			sdk::colour f_glow_clr = sdk::colour(150, 200, 60);
			sdk::colour e_glow_clr = sdk::colour(150, 200, 60);
		} glow;

		struct {
			struct {
				bool visible;
				bool invisible;
				int alpha = 100;

				sdk::colour vis_chams_clr = sdk::colour(150, 200, 60);
				sdk::colour invis_chams_clr = sdk::colour(60, 180, 220);
			} enemy;

			struct {
				bool visible;
				bool invisible;
				int alpha = 100;

				sdk::colour vis_chams_clr = sdk::colour(250, 200, 0);
				sdk::colour invis_chams_clr = sdk::colour(250, 50, 0);
			} friendly;

			struct {
				bool check;
				bool scope_blend;
				int alpha = 100;;

				sdk::colour colour = sdk::colour(250, 250, 200);
			} local;
		} chams;

	} players;

	struct m_movement {
		bool bhop;
		bool auto_strafer;
	} movement;

	struct m_visuals {
		bool spectators;
		bool remove_scope;
		bool remove_flash;
		bool remove_smoke;

		int fov = 90;
		bool override_fov_scoped;

		struct m_world {
			bool night;
			bool fullbright;
		} world;

	} visuals;

}; extern cfg g_config;

class value {
public:
	template <typename t = float>
	t get() {
		return (t)m_value;
	}

	template <typename t = float>
	void set(t in) {
		m_value = (double)in;
	}

private:
	double m_value = 0.0;
};

extern std::map< std::string, value > g_cfg;

void save(std::string name);

void load(std::string name);

