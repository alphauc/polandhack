#pragma once

#include "../misc/vector.hh"
#include "base_interface.hh"
#include "net_channel_info.hh"

namespace sdk {

	struct info_t {
		int64_t __pad0;
		union {
			int64_t xuid;
			struct {
				int xuid_low;
				int xuid_high;
			};
		};
		char player_name[ 128 ];
		int user_id;
		char g_uid[ 33 ];
		unsigned int friends_id;
		char friends_name[ 128 ];
		bool fake_player;
		bool is_hltv;
		unsigned int custom_files[ 4 ];
		unsigned char files_downloaded;
	};

	class engine_client : public base_interface {
	public:
		auto get_local_player( ) -> int32_t {
			return this->get_vfunc<12, int32_t>( this );
		};
		auto get_player_info( int index, info_t *info ) -> void {
			this->get_vfunc<8, void, int32_t, info_t *>( this, index, info );
		};
		auto get_screen_size( int& w, int&h ) -> void {
			this->get_vfunc<5, void, int&, int&>( this, w, h );
		};
		vec3 get_view_angles( ) {
			vec3 temp;
			using original_fn = void( __thiscall* )( engine_client*, vec3& );
			( *( original_fn** )this )[ 18 ]( this, temp );
			return temp;
		}
		auto is_in_game( ) -> bool {
			return this->get_vfunc<26, bool>( this );
		};
		auto is_connected( ) -> bool {
			return this->get_vfunc<27, bool>( this );
		};
		auto channel_info( ) -> net_channel_info* {
			return this->get_vfunc<78, net_channel_info*>( this );
		};
		auto execute_cmd( const char *str ) -> void {
			this->get_vfunc<108, void, const char*>( this, str );
		};
		auto get_player_for_user_id( int id ) -> int {
			return this->get_vfunc<9, int, int>( this, id );
		};

	};

}