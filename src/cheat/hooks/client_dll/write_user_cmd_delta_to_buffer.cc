#include "../hook_manager.hh"
#include "../../cfg.hh"

bool __fastcall hooks::write_cmd( void *ecx, void *edx, int slot, void *buf, int from, int to, bool new_cmd ) {
	return client_dll->get_original<hooks::write_cmd_fn>( 23 )( edx, slot, buf, from, to, new_cmd);
}