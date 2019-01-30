#pragma once

#include "../sdk/classes/entity.hh"
#include "../sdk/classes/user_cmd.hh"
#include "../sdk/misc/vector.hh"

#define IN_ATTACK		(1 << 0)
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_ALT2			(1 << 15)
#define IN_SCORE		(1 << 16)
#define IN_SPEED		(1 << 17)
#define IN_WALK			(1 << 18)
#define IN_ZOOM			(1 << 19)
#define IN_WEAPON1		(1 << 20)
#define IN_WEAPON2		(1 << 21)
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)
#define IN_GRENADE2		(1 << 24)
#define	IN_LOOKSPIN		(1 << 25)

#define	FL_ONGROUND				(1 << 0)	
#define FL_DUCKING				(1 << 1)	
#define	FL_WATERJUMP			(1 << 3)	
#define FL_ONTRAIN				(1 << 4) 
#define FL_INRAIN				(1 << 5)	
#define FL_FROZEN				(1 << 6) 
#define FL_ATCONTROLS			(1 << 7) 
#define	FL_CLIENT				(1 << 8)	
#define FL_FAKECLIENT			(1 << 9)	
#define	FL_INWATER				(1 << 10)

namespace csgo {

	extern sdk::vec3 unpredicted_velocity;
	extern sdk::vec3 real_angles;
	extern sdk::vec3 fake_angles;
	extern sdk::vec3 lowerbody_angles;

	extern bool can_shoot_someone;
	extern bool send_packet;
	extern bool fakelagging;
	extern bool initialized;
    extern void *dll_instance;

    extern sdk::base_entity *local_player;

    void entry_point(void *instance);
    void shutdown();

}