#include "menu.hh"
#include "../../utilities/config.hh"
#include "../../utilities/console.hh"
#include "../cheat.hh"

namespace menu {
	c_ui ui;

	void c_ui::update_ui( ) {
		// Let's see if the menu was toggled.	
		static bool is_down = false;
		static bool is_clicked = false;

		if ( g_input.key_down( VK_INSERT ) ) {
			is_clicked = false;
			is_down = true;
		}
		else if ( !g_input.key_down( VK_INSERT ) && is_down ) {
			is_clicked = true;
			is_down = false;
		}
		else {
			is_clicked = false;
			is_down = false;
		}

		if ( is_clicked )
			g_config.menu.m_open = !g_config.menu.m_open;

		// Update menu opacity.
		constexpr float fade_ratio = 1.0f / 0.70f;
		const     float  cur_ratio = fade_ratio * interfaces::global_vars->frame_time;
		const     int    cur_inc = ( int )( cur_ratio * 255.0f );

		int new_alpha = g_config.menu.m_alpha;
		new_alpha += g_config.menu.m_open ? cur_inc : -cur_inc;
		new_alpha = std::clamp< int >( new_alpha, 0, 255 );

		g_config.menu.m_alpha = ( size_t )new_alpha;

		// Update input every frame.
		g_input.on_frame( );
	}

	void c_ui::initialize( ) {
		// Implement an object container.
		m_container = std::make_shared< object::c_container >( );

		auto form = std::make_shared< object::c_form >( 50, 50, 619, 489 ); {

			auto tab_control = std::make_shared< object::c_tab_control >( ); {

				auto aimbot_tab = std::make_shared< object::c_tab >( "aimbot" ); {
					auto container = std::make_shared< object::c_tab_container >( ); {
						container->push( std::make_shared<object::c_checkbox>( "enable", &g_config.aimbot.enabled ) );

						std::vector< object::multi_item_t > hitboxes{
							{ "head", &g_config.aimbot.hitscan_head },
							{ "body", &g_config.aimbot.hitscan_body},
							{ "stomach", &g_config.aimbot.hitscan_stomach},
							{ "arms", &g_config.aimbot.hitscan_arms},
							{ "legs", &g_config.aimbot.hitscan_legs},
							{ "feets", &g_config.aimbot.hitscan_feets},
						};

						std::vector< object::multi_item_t > multipoint{
							{ "head", &g_config.aimbot.hitscan_head_multipoint },
							{ "body", &g_config.aimbot.hitscan_body_multipoint},
							{ "stomach", &g_config.aimbot.hitscan_stomach_multipoint},
							{ "arms", &g_config.aimbot.hitscan_arms_multipoint},
							{ "legs", &g_config.aimbot.hitscan_legs_multipoint},
							{ "feets", &g_config.aimbot.hitscan_feets_multipoint},
						};

						container->push( std::make_shared<object::c_multi_dropdown>( "hitboxes", hitboxes ) );
						container->push( std::make_shared<object::c_multi_dropdown>( "multipoint", multipoint ) );
						container->push( std::make_shared<object::c_slider<int> >( "pointscale head", 1, 100, &g_config.aimbot.pointscale_head ) );
						container->push( std::make_shared<object::c_slider<int> >( "pointscale body", 1, 100, &g_config.aimbot.pointscale ) );

						container->push( std::make_shared<object::c_slider<int> >( "minimal damage", 1, 100, &g_config.aimbot.min_dmg ) );
						container->push( std::make_shared<object::c_slider<int> >( "minimal damage autowall", 1, 100, &g_config.aimbot.min_awall_dmg ) );

					}
					container->split( ); {

						std::vector< object::multi_item_t > body_aim{
							{ "always", &g_config.aimbot.baim_always },
							{ "unplugged", &g_config.aimbot.baim_if_unplugged },
							{ "in air", &g_config.aimbot.baim_if_air },
							{ "health", &g_config.aimbot.baim_if_x_damage_enabled },
						};

						container->push( std::make_shared<object::c_checkbox>( "hitchance", &g_config.aimbot.hitchance_enabled ) );

						auto hitchance_slider = std::make_shared<object::c_slider<int> >( "hitchance amount", 1, 100, &g_config.aimbot.hitchance );
						hitchance_slider->set_condition( [ ] { return g_config.aimbot.hitchance_enabled; } );
						container->push( hitchance_slider );

						auto delay_shot = std::make_shared<object::c_slider<float> >( "delay shot", 0.f, 0.2f, &g_config.aimbot.delay_shot ); {
							delay_shot->set_measure( "s" );
						}
						container->push( delay_shot );

						container->push( std::make_shared<object::c_multi_dropdown>( "body aim", body_aim ) );

						if ( g_config.aimbot.baim_if_x_damage_enabled ) {
							auto baim_health = std::make_shared<object::c_slider<int> >( "body aim below", 1, 125, &g_config.aimbot.baim_if_x_damage ); {
								baim_health->set_measure( "hp" );
							}
							container->push( baim_health );
						}
					}
					aimbot_tab->push( container );
				}
				tab_control->push( aimbot_tab );

				auto antiaim_tab = std::make_shared< object::c_tab >( "anti-aim" ); {
					auto container = std::make_shared< object::c_tab_container >( ); {
						std::vector< std::string > anti_aim_base{
							"stand",
							"move",
							"in-air",
						};

						std::vector< std::string > anti_aim_pitch{
							"off",
							"down",
							"up",
						};

						std::vector< std::string > anti_aim_yaw{
							"off",
							"backwards",
							"rotate",
							"sideways",
							"switch",
						};

						container->push( std::make_shared< object::c_checkbox >( "enable", &g_config.anti_aim.enable ) );
						container->push( std::make_shared< object::c_checkbox >( "edge", &g_config.anti_aim.edge ) );
						container->push( std::make_shared< object::c_dropdown >( "base", &g_config.anti_aim.base, anti_aim_base ) );
						container->push( std::make_shared< object::c_dropdown >( "pitch", &g_config.anti_aim.pitch, anti_aim_pitch ) );

						/* AAAAAAAAAAAAAAAAAAAAAA-UGLY CODE WARNING-AAAAAAAAAAAAAAAAAAAAAA*/
						{ // stand
							auto yaw = std::make_shared< object::c_dropdown >( "yaw stand", &g_config.anti_aim.stand.yaw, anti_aim_yaw );
							yaw->set_condition( [ ] { return g_config.anti_aim.base != 1 && g_config.anti_aim.base != 2; } );
							container->push( yaw );
						}
						{ // moving
							auto yaw = std::make_shared< object::c_dropdown >( "yaw move", &g_config.anti_aim.move.yaw, anti_aim_yaw );
							yaw->set_condition( [ ] { return g_config.anti_aim.base != 0 && g_config.anti_aim.base != 2; } );
							container->push( yaw );
						}
						{ // in-air
							auto yaw = std::make_shared< object::c_dropdown >( "yaw air", &g_config.anti_aim.air.yaw, anti_aim_yaw );
							yaw->set_condition( [ ] { return g_config.anti_aim.base != 0 && g_config.anti_aim.base != 1; } );
							container->push( yaw );
						}

						auto fake_body = std::make_shared< object::c_checkbox >( "fake body", &g_config.anti_aim.fake_body ); {
							fake_body->set_condition( [ ] { return g_config.anti_aim.stand.yaw > 0; } );
							container->push( fake_body );
						}
					}
					container->split( ); {
						std::vector< std::string > anti_aim_fake_yaw{
							"off",
							"backwards",
							"rotate",
							"sideways",
							"opposite",
						};

						std::vector< object::multi_item_t > fakelag_activation{
							{ "standing", &g_config.anti_aim.fakelag_check_standing },
							{ "moving", &g_config.anti_aim.fakelag_check_moving },
							{ "air", &g_config.anti_aim.fakelag_check_air},
						};

						std::vector< std::string > fakelag_type{
							"factor",
							"dynamic",
							"break",
						};

						container->push( std::make_shared< object::c_dropdown >( "fake yaw", &g_config.anti_aim.fake_yaw, anti_aim_fake_yaw ) );

						container->push( std::make_shared< object::c_multi_dropdown >( "fakelag activation", fakelag_activation ) );
						container->push( std::make_shared<object::c_dropdown>( "type", &g_config.anti_aim.fakelag_type, fakelag_type ) );

						auto fakelag_packets = std::make_shared < object::c_slider< int > >( "packets", 0, 14, &g_config.anti_aim.fakelag_packets ); {
							fakelag_packets->set_measure( "p" );
						}
						container->push( fakelag_packets );

					}
					antiaim_tab->push( container );
				}
				tab_control->push( antiaim_tab );

				auto players_tab = std::make_shared< object::c_tab >( "players" ); {
					auto container = std::make_shared< object::c_tab_container >( ); {
						std::vector< object::multi_item_t > player_flags{
							{ "money", &g_config.players.flags.e_money },
							{ "armor", &g_config.players.flags.e_armour },
							{ "scoped", &g_config.players.flags.e_zoom },
							{ "bomb", &g_config.players.flags.e_bomb },
						};

						std::vector< object::multi_item_t > friendly_flags{
							{ "money", &g_config.players.flags.f_money },
							{ "armor", &g_config.players.flags.f_armour },
							{ "scoped", &g_config.players.flags.f_zoom },
							{ "bomb", &g_config.players.flags.f_bomb },
						};


						std::vector< object::multi_item_t > player_boxes{
							{ "enemy", &g_config.players.boxes.enemy },
							{ "friendly", &g_config.players.boxes.friendly },
						};

						std::vector< object::multi_item_t > player_name{
							{ "enemy", &g_config.players.name.enemy },
							{ "friendly", &g_config.players.name.friendly },
						};

						std::vector< object::multi_item_t > player_weapon{
							{ "enemy", &g_config.players.weapon.enemy },
							{ "friendly", &g_config.players.weapon.friendly },
						};

						std::vector< object::multi_item_t > player_health{
							{ "enemy", &g_config.players.health.enemy },
							{ "friendly", &g_config.players.health.friendly },
						};

						container->push( std::make_shared< object::c_multi_dropdown >( "boxes", player_boxes ) );
						container->push( std::make_shared< object::c_colorpicker >( "box enemy color", &g_config.players.boxes.e_box_clr ) );
						container->push( std::make_shared< object::c_colorpicker >( "box friendly color", &g_config.players.boxes.f_box_clr ) );

						container->push( std::make_shared< object::c_checkbox >( "dormant enemies", &g_config.players.dormant ) );
						container->push( std::make_shared< object::c_checkbox >( "enemy offscreen esp", &g_config.players.pov_arrows ) );
						container->push( std::make_shared< object::c_colorpicker >( "offscreen esp color", &g_config.players.pov_arrows_clr ) );

						container->push( std::make_shared< object::c_multi_dropdown >( "name", player_name ) );
						container->push( std::make_shared< object::c_colorpicker >( "name color", &g_config.players.name.name_clr ) );

						container->push( std::make_shared< object::c_multi_dropdown >( "health", player_health ) );

						container->push( std::make_shared< object::c_multi_dropdown >( "flags enemy", player_flags ) );
						container->push( std::make_shared< object::c_multi_dropdown >( "flags friendly", friendly_flags ) );

						container->push( std::make_shared< object::c_multi_dropdown >( "weapon", player_weapon ) );

						container->push( std::make_shared< object::c_checkbox >( "ammo", &g_config.players.weapon.ammo ) );
						container->push( std::make_shared< object::c_colorpicker >( "color", &g_config.players.weapon.ammo_clr ) );
					}
					container->split( ); {
						std::vector< object::multi_item_t > player_skeleton{
							{ "enemy", &g_config.players.skeleton.enemy },
							{ "friendly", &g_config.players.skeleton.friendly },
						};

						std::vector< object::multi_item_t > player_glow{
							{ "enemy", &g_config.players.glow.enemy },
							{ "friendly", &g_config.players.glow.friendly },
						};


						std::vector< object::multi_item_t > enemy_chams{
							{ "visible", &g_config.players.chams.enemy.visible },
							{ "invisible", &g_config.players.chams.enemy.invisible },
						};

						std::vector< object::multi_item_t > friendly_chams{
							{ "visible", &g_config.players.chams.friendly.visible },
							{ "invisible", &g_config.players.chams.friendly.invisible },
						};

						std::vector< std::string > glow_style{
							"bloom",
							"outline",
						};

						container->push( std::make_shared< object::c_multi_dropdown >( "glow", player_glow ) );
						container->push( std::make_shared< object::c_dropdown >( "type", &g_config.players.glow.style, glow_style ) );
						container->push( std::make_shared< object::c_colorpicker >( "enemy colour", &g_config.players.glow.e_glow_clr ) );
						container->push( std::make_shared< object::c_colorpicker >( "friendly colour", &g_config.players.glow.f_glow_clr ) );

						auto glow_alpha = std::make_shared < object::_c_slider< int > >( 0, 100, &g_config.players.glow.alpha ); {
							glow_alpha->set_measure( "%" );
						}
						container->push( glow_alpha );

						container->push( std::make_shared< object::c_multi_dropdown >( "chams enemy", enemy_chams ) );
						container->push( std::make_shared< object::c_colorpicker >( "colour visible", &g_config.players.chams.enemy.vis_chams_clr ) );
						container->push( std::make_shared< object::c_colorpicker >( "colour invisible", &g_config.players.chams.enemy.invis_chams_clr ) );

						auto chams_enemy_alpha = std::make_shared < object::_c_slider< int > >( 0, 100, &g_config.players.chams.enemy.alpha ); {
							chams_enemy_alpha->set_measure( "%" );
						}
						container->push( chams_enemy_alpha );

						container->push( std::make_shared< object::c_multi_dropdown >( "chams friendly", friendly_chams ) );
						container->push( std::make_shared< object::c_colorpicker >( "colour visible", &g_config.players.chams.friendly.vis_chams_clr ) );
						container->push( std::make_shared< object::c_colorpicker >( "colour invisible", &g_config.players.chams.friendly.invis_chams_clr ) );

						auto chams_friendly_alpha = std::make_shared < object::_c_slider< int > >( 0, 100, &g_config.players.chams.friendly.alpha ); {
							chams_friendly_alpha->set_measure( "%" );
						}
						container->push( chams_friendly_alpha );

						container->push( std::make_shared< object::c_checkbox >( "chams local", &g_config.players.chams.local.check ) );
						container->push( std::make_shared< object::c_colorpicker >( "colour", &g_config.players.chams.local.colour ) );

						auto chams_local_alpha = std::make_shared < object::_c_slider< int > >( 0, 100, &g_config.players.chams.local.alpha ); {
							chams_local_alpha->set_measure( "%" );

						}
						container->push( chams_local_alpha );

						container->push( std::make_shared< object::c_checkbox >( "blend when scoped", &g_config.players.chams.local.scope_blend ) );

					}
					players_tab->push( container );
				}
				tab_control->push( players_tab );

				auto visuals_tab = std::make_shared< object::c_tab >( "visuals" ); {
					auto container = std::make_shared< object::c_tab_container >( ); {
						std::vector< object::multi_item_t > world_items{
							{ "nightmode", &g_config.visuals.world.night },
							{ "fullbright", &g_config.visuals.world.fullbright },
						};

						container->push( std::make_shared< object::c_checkbox >( "show spectator list", &g_config.visuals.spectators ) );

						container->push( std::make_shared< object::c_multi_dropdown >( "world", world_items ) );
					}
					container->split( ); {

						container->push( std::make_shared< object::c_checkbox >( "remove scope", &g_config.visuals.remove_scope ) );
						container->push( std::make_shared< object::c_checkbox >( "remove flash", &g_config.visuals.remove_flash ) );
						container->push( std::make_shared< object::c_checkbox >( "remove smoke", &g_config.visuals.remove_smoke ) );

						auto fov_slider = std::make_shared < object::c_slider< int > >( "fov", 0, 120, &g_config.visuals.fov ); {
							fov_slider->set_measure( "°" );

						}
						container->push( fov_slider );

						container->push( std::make_shared< object::c_checkbox >( "override fov while scoping", &g_config.visuals.override_fov_scoped ) );
					}
					visuals_tab->push( container );
				}
				tab_control->push( visuals_tab );

				auto movement_tab = std::make_shared< object::c_tab >( "movement" ); {
					auto container = std::make_shared< object::c_tab_container >( ); {
						container->push( std::make_shared< object::c_checkbox >( "automatic jump", &g_config.movement.bhop ) );
						container->push( std::make_shared< object::c_checkbox >( "automatic strafe", &g_config.movement.auto_strafer ) );
					}
					container->split( ); {

					}
					movement_tab->push( container );
				}
				tab_control->push( movement_tab );

				auto skins_tab = std::make_shared< object::c_tab >( "skins" ); {
					auto container = std::make_shared< object::c_tab_container >( ); {

					}
					container->split( ); {
					}
					skins_tab->push( container );
				}
				tab_control->push( skins_tab );

				auto misc_tab = std::make_shared< object::c_tab >( "misc" ); {
					auto container = std::make_shared< object::c_tab_container >( ); {
					}
					container->split( ); {
						container->push( std::make_shared< object::c_colorpicker >( "menu accent colour", &g_config.menu.m_accent ) );
						//	container->push(std::make_shared< object::c_checkbox >("render rainbow gradient", &g_config.menu.rainbow));
					}
					misc_tab->push( container );
				}
				tab_control->push( misc_tab );

				auto config_tab = std::make_shared< object::c_tab >( "config" ); {
					auto container = std::make_shared< object::c_tab_container >( ); {
					}
					container->split( ); {
					}
					config_tab->push( container );
				}
				tab_control->push( config_tab );
			}
			form->push( tab_control );
		}
		m_container->push( form );

		m_initialized = true;
	}

	void c_ui::on_frame( ) {
		//console::log("stage 2: start");
		if ( !m_initialized || !csgo::initialized )
			return;

		// Make sure we update the UI every frame.
		update_ui( );

		// Draw the UI.
		if ( g_config.menu.m_alpha >= 5 )
			m_container->draw( );
	}
}