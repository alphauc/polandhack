#include "interface_manager.hh"
#include "utilities.hh"

sdk::client_dll *interfaces::client_dll;
sdk::client_mode *interfaces::client_mode;
sdk::engine_client *interfaces::engine_client;
sdk::entity_list *interfaces::entity_list;
sdk::vgui_panel *interfaces::vgui_panel;
sdk::vgui_surface *interfaces::vgui_surface;
sdk::debug_overlay *interfaces::debug_overlay;
sdk::d3d_device_9 *interfaces::d3d_device;
sdk::global_vars *interfaces::global_vars;
sdk::render_view *interfaces::render_view;
sdk::material_system *interfaces::material_system;
sdk::model_render *interfaces::model_render;
sdk::glow_object_manager *interfaces::glow_object_manager;
sdk::client_state *interfaces::client_state;
sdk::input *interfaces::input;
sdk::player_game_movement *interfaces::player_game_movement;
sdk::player_prediction *interfaces::player_prediction;
sdk::player_move_helper *interfaces::player_move_helper;
sdk::trace* interfaces::trace;
sdk::convar* interfaces::convar;
sdk::game_event* interfaces::game_event;
sdk::physic_props* interfaces::physic_props;
sdk::model_information* interfaces::model_information;
sdk::engine_sound* interfaces::engine_sound;

void interfaces::initialize( ) {
	client_dll = get_interface<sdk::client_dll, search_type::exact>( "client_panorama.dll", "VClient018" );
	engine_client = get_interface<sdk::engine_client, search_type::exact>( "engine.dll", "VEngineClient014" );
	entity_list = get_interface<sdk::entity_list, search_type::exact>( "client_panorama.dll", "VClientEntityList003" );
	vgui_panel = get_interface<sdk::vgui_panel, search_type::exact>( "vgui2.dll", "VGUI_Panel009" );
	vgui_surface = get_interface<sdk::vgui_surface, search_type::exact>( "vguimatsurface.dll", "VGUI_Surface031" );
	debug_overlay = get_interface<sdk::debug_overlay, search_type::exact>( "engine.dll", "VDebugOverlay004" );
	render_view = get_interface<sdk::render_view, search_type::exact>( "engine.dll", "VEngineRenderView014" );
	material_system = get_interface<sdk::material_system, search_type::exact>( "materialsystem.dll", "VMaterialSystem080" );
	model_render = get_interface<sdk::model_render, search_type::exact>( "engine.dll", "VEngineModel016" );
	player_game_movement = get_interface<sdk::player_game_movement, search_type::exact>( "client_panorama.dll", "GameMovement001" );
	player_prediction = get_interface<sdk::player_prediction, search_type::exact>( "client_panorama.dll", "VClientPrediction001" );
	trace = get_interface<sdk::trace, search_type::exact>( "engine.dll", "EngineTraceClient004" );
	convar = get_interface<sdk::convar, search_type::exact>( "vstdlib.dll", "VEngineCvar007" );
	game_event = get_interface<sdk::game_event, search_type::exact>( "engine.dll", "GAMEEVENTSMANAGER002" );
	physic_props = get_interface<sdk::physic_props, search_type::exact>( "vphysics.dll", "VPhysicsSurfaceProps001" );
	model_information = get_interface<sdk::model_information, search_type::bruteforce>( "engine.dll", "VModelInfoClient" );
	engine_sound = get_interface<sdk::engine_sound, search_type::exact>( "engine.dll", "IEngineSoundClient003" );

	glow_object_manager = *reinterpret_cast< sdk::glow_object_manager** >( utilities::find_occurrence( "client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01" ) + 0x3 );
	d3d_device = ( sdk::d3d_device_9 * ) ( **( uintptr_t ** )( utilities::find_occurrence( "shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C" ) + 1 ) );
	input = *( sdk::input** )( utilities::find_occurrence( "client_panorama.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" ) + 1 );
	player_move_helper = **( sdk::player_move_helper*** )( utilities::find_occurrence( "client_panorama.dll", "8B 0D ? ? ? ? 8B 46 08 68" ) + 2 );

	client_state = **reinterpret_cast< sdk::client_state*** > ( ( *reinterpret_cast< uintptr_t** > ( engine_client ) )[ 12 ] + 0x10 );
	client_mode = **reinterpret_cast< sdk::client_mode*** >( ( *reinterpret_cast< uintptr_t** >( client_dll ) )[ 10 ] + 5 );
	global_vars = **reinterpret_cast< sdk::global_vars*** >( ( *reinterpret_cast< uintptr_t** >( client_dll )[ 0 ] + 27 ) );
}
