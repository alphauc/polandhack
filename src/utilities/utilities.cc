#include "utilities.hh"
#include "interface_manager.hh"

uintptr_t utilities::find_occurrence(const std::string &module_name, const std::string &pattern) {
    const char* pat = pattern.c_str();
    DWORD firstMatch = 0;
    DWORD rangeStart = (DWORD)GetModuleHandleA(module_name.c_str());
    MODULEINFO miModInfo;
    GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
    DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
    for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
    {
        if (!*pat)
            return firstMatch;

        if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == GET_BYTE(pat))
        {
            if (!firstMatch)
                firstMatch = pCur;

            if (!pat[2])
                return firstMatch;

            if (*(PBYTE)pat != '\?')
                pat += 3;

            else
                pat += 2;
        }
        else
        {
            pat = pattern.c_str();
            firstMatch = 0;
        }
    }
    return 0u;
}

unsigned int utilities::find_pattern(const char* module_name, const BYTE* mask, const char* mask_string) {
	/// Get module address
	const unsigned int module_address = reinterpret_cast<unsigned int>(GetModuleHandle(module_name));

	/// Get module information to the size
	MODULEINFO module_info;
	GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(module_address), &module_info, sizeof(MODULEINFO));

	auto IsCorrectMask = [](const unsigned char* data, const unsigned char* mask, const char* mask_string) -> bool {
		for (; *mask_string; ++mask_string, ++mask, ++data)
			if (*mask_string == 'x' && *mask != *data)
				return false;

		return (*mask_string) == 0;
	};

	/// Iterate until we find a matching mask
	for (unsigned int c = 0; c < module_info.SizeOfImage; c += 1) {
		/// does it match?
		if (IsCorrectMask(reinterpret_cast<unsigned char*>(module_address + c), mask, mask_string))
			return (module_address + c);
	}

	return 0;
}

float utilities::curtime() {

	auto local_player = interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());
	if (!local_player)
		return 0.f;

	return local_player->tick_base() * interfaces::global_vars->interval_per_tick;
}

float utilities::latency() {

	auto nci = interfaces::engine_client->channel_info();
	if (!nci)
		return 0.f;

	return nci->GetLatency(FLOW_OUTGOING);
}

bool utilities::can_shoot(bool right_click) {
	auto local_player = interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());
	if (!local_player || local_player->health() <= 0 || *local_player->next_attack() > curtime())
		return false;

	auto weapon = reinterpret_cast<sdk::base_weapon*>(interfaces::entity_list->get_entity(local_player->active_weapon_index()));
	if (!weapon)
		return false;

	/// special shit for nades
	/*if (is_grenade(weapon)) { //L8R  TODO********+
		if (weapon->IsPinPulled())
			return false;

		if (weapon->GetThrowTime() <= 0 || weapon->GetThrowTime() > curtime())
			return false;

		return true;
	}*/

	if (right_click) {
		if (*weapon->next_secondary_attack() > curtime())
			return false;
	}
	else {
		if (*weapon->next_primary_attack() > curtime())
			return false;

		/// revolver
		if (weapon->item_definition_index() == sdk::base_weapon::weapon_class_id::WEAPON_REVOLVER && *weapon->post_pone_fire_ready_time() > curtime())
			return false;
	}

	return true;
}

const wchar_t* utilities::to_wchar(const char* string) {
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, string);
	_vsnprintf_s(buf, sizeof(buf), string, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	return wbuf;
}