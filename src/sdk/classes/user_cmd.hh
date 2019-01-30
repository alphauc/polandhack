#pragma once

#include "../misc/crc32.hh"
#include "../misc/vector.hh"

namespace sdk {

	class user_cmd {
	public:
		user_cmd() { };
		virtual ~user_cmd() { };

	/*	crc32_t checksum(void) const
		{
			crc32_t crc;
			crc32_init(&crc);

			crc32_process_buffer(&crc, &cmd_number, sizeof(cmd_number));
			crc32_process_buffer(&crc, &tick_count, sizeof(tick_count));
			crc32_process_buffer(&crc, &view_angles, sizeof(view_angles));
			crc32_process_buffer(&crc, &aim_direction, sizeof(aim_direction));
			crc32_process_buffer(&crc, &forward_move, sizeof(forward_move));
			crc32_process_buffer(&crc, &side_move, sizeof(side_move));
			crc32_process_buffer(&crc, &up_move, sizeof(up_move));
			crc32_process_buffer(&crc, &buttons, sizeof(buttons));
			crc32_process_buffer(&crc, &impulse, sizeof(impulse));
			crc32_process_buffer(&crc, &weapon_select, sizeof(weapon_select));
			crc32_process_buffer(&crc, &weapon_subtype, sizeof(weapon_subtype));
			crc32_process_buffer(&crc, &random_seed, sizeof(random_seed));
			crc32_process_buffer(&crc, &mouse_dx, sizeof(mouse_dx));
			crc32_process_buffer(&crc, &mouse_dy, sizeof(mouse_dy));

			crc32_final(&crc);
			return crc;
		}*/

		int cmd_number;
		int tick_count;
		sdk::vec3 view_angles;
		sdk::vec3 aim_direction;
		float forward_move;
		float side_move;
		float up_move;
		int buttons;
		unsigned char impulse;
		int weapon_select;
		int weapon_subtype;
		int random_seed;
		short mouse_dx;
		short mouse_dy;
		bool has_been_predicted;
		char pad_0x4c[0x18];
	};

	class verified_user_cmd
	{
	public:
		user_cmd m_cmd;
		crc32_t  m_crc;
	};

}