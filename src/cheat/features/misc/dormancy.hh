#pragma once
#include "../../../utilities/interface_manager.hh"

struct dormancy_data
{
	sdk::base_entity* entity;
	bool	prevstate;
	bool	set;
	int		flags;
	sdk::vec3	oldpos = sdk::vec3( 0.f, 0.f, 0.f );
	struct {
		int time;
	} sounddata;
};

namespace features::misc::dormancy {
	void position_correction( int stage );
	bool set( const int index );

	bool viable( sdk::base_entity* entity );

	void sound_dormancy( );
	void adjust_player( const int index );
	void restore_sound_dormancy( );
	bool valid_sound( sdk::sound_info& sound );

	inline sdk::utl_vector<sdk::sound_info> soundbuffer;
	inline sdk::utl_vector<sdk::sound_info> sound_list;

	inline dormancy_data players[ 65 ];
}