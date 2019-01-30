#include <cstdint>
#include <windows.h>

#include "cheat/cheat.hh"

bool __stdcall DllMain(void *instance, uintptr_t reason, void *reserved) {
    DisableThreadLibraryCalls((HMODULE) instance);

    switch (reason) {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) csgo::entry_point, instance, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        csgo::shutdown();
        break;

    }

    return true;
}