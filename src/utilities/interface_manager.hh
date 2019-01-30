#pragma once

#include <windows.h>
#include <cstdint>
#include <string>

#include "../sdk/interfaces/render_view.hh"
#include "../sdk/interfaces/client_dll.hh"
#include "../sdk/interfaces/client_mode.hh"
#include "../sdk/interfaces/engine_client.hh"
#include "../sdk/interfaces/global_vars.hh"
#include "../sdk/interfaces/entity_list.hh"
#include "../sdk/interfaces/vgui_panel.hh"
#include "../sdk/interfaces/vgui_surface.hh"
#include "../sdk/interfaces/model_render.hh"
#include "../sdk/interfaces/debug_overlay.hh"
#include "../sdk/interfaces/game_prediction.hh"
#include "../sdk/interfaces/material_system.hh"
#include "../sdk/interfaces/physics_props.hh"
#include "../sdk/interfaces/input.hh"
#include "../sdk/interfaces/networking.hh"
#include "../sdk/interfaces/glow_object_manager.hh"
#include "../sdk/misc/definitions.hh"
#include "../sdk/classes/trace.hh"
#include "../sdk/interfaces/convar.hh"
#include "../sdk/interfaces/model_info.hh"
#include "../sdk/interfaces/game_events.hh"
#include "../sdk/interfaces/engine_sound.hh"

namespace interfaces {

    enum class search_type { exact, bruteforce };

    template <typename ret, search_type type>
    ret *get_interface(std::string module_name, std::string interface_name) {
        using create_interface_fn = void *( *)(const char *, int *);
        create_interface_fn iface_fn = (create_interface_fn)
                            GetProcAddress(GetModuleHandle(module_name.c_str()), "CreateInterface");

        if (iface_fn) {
            void *result = nullptr;
            
            if (type == search_type::exact) {
                result = iface_fn(interface_name.c_str(), nullptr);
				printf("0x%p -> %s\n", result, interface_name.c_str());

            }
            else {
                char buf[128];
                
                for (uint32_t i = 0; i <= 100; i++) {
                    memset((void *)buf, 0, sizeof buf);
                    sprintf_s(buf, "%s%03i", interface_name.c_str(), i);

                    result = iface_fn(interface_name.c_str(), nullptr);

                    if (result)
                        break;
                }
            }

            return reinterpret_cast<ret*>(result);
        }

        return reinterpret_cast<ret *>(nullptr);
    }

    extern sdk::client_dll *client_dll;
	extern sdk::client_mode *client_mode;
    extern sdk::engine_client *engine_client;
    extern sdk::entity_list *entity_list;
    extern sdk::vgui_panel *vgui_panel;
    extern sdk::vgui_surface *vgui_surface;
    extern sdk::debug_overlay *debug_overlay;
    extern sdk::d3d_device_9 *d3d_device;
	extern sdk::global_vars *global_vars;
	extern sdk::render_view *render_view;
	extern sdk::material_system *material_system;
	extern sdk::model_render *model_render;
	extern sdk::glow_object_manager *glow_object_manager;
	extern sdk::client_state *client_state;
	extern sdk::input *input;
	extern sdk::player_game_movement *player_game_movement;
	extern sdk::player_prediction *player_prediction;
	extern sdk::player_move_helper *player_move_helper;
	extern sdk::trace* trace;
	extern sdk::convar* convar;
	extern sdk::game_event* game_event;
	extern sdk::physic_props* physic_props;
	extern sdk::model_information* model_information;
	extern sdk::engine_sound* engine_sound;

    void initialize();

}