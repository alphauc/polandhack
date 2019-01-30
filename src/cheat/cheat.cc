#include "cheat.hh"

#include "../utilities/console.hh"
#include "../utilities/config.hh"
#include "../utilities/interface_manager.hh"
#include "../utilities/netvar_manager.hh"
#include "../utilities/renderer.hh"
#include "gui/menu.hh"
#include "hooks/hook_manager.hh"

sdk::vec3 csgo::unpredicted_velocity;
sdk::vec3 csgo::real_angles;
sdk::vec3 csgo::fake_angles;
sdk::vec3 csgo::lowerbody_angles;

bool csgo::send_packet;
bool csgo::fakelagging = false;
bool csgo::initialized = false;
bool csgo::can_shoot_someone;
void *csgo::dll_instance;
sdk::base_entity *csgo::local_player;

void csgo::entry_point(void *instance) {
	while (!GetModuleHandleA("serverbrowser.dll"))
		Sleep(200);

    csgo::dll_instance = instance;

    console::allocate();
    console::title("poland hack");

	menu::ui.initialize();
	config::initialize();
	interfaces::initialize();
	g_input.initialise();
    netvars::initialize();
    hooks::initialize();

    while (!g_input.key_down(VK_END))
        utilities::wait(std::chrono::milliseconds(125));

    FreeLibraryAndExitThread((HMODULE) instance, 0);
}

void csgo::shutdown() {
	g_input.shutdown();
	config::shutdown();
    hooks::shutdown();
    render::shutdown();
    console::detach();
}