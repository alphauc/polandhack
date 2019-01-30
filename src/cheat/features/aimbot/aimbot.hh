#pragma once
#include "../../cheat.hh"
#include "../../../utilities/interface_manager.hh"
#include "../../../utilities/math.hh"
#include "backtracking.hh"

namespace features::ragebot {
	class automatic_shoot_at_target_bot
	{
	public:
		void execute( sdk::user_cmd* cmd );
		sdk::vec3 GetHitboxPosition( sdk::base_entity* entity, int hitbox_index );
	private:
		struct PlayerAimbotInfo
		{
			int head_damage = 0, hitscan_damage = 0, tick, extrapolted_ticks;
			sdk::vec3 head_position, hitscan_position;
			features::backtracking::backtracking_record backtrack_record;
		};

	private:
		PlayerAimbotInfo player_aimbot_info[ 64 ];

	public:
		PlayerAimbotInfo GetPlayerAimbotInfo( sdk::base_entity* entity ) {
			return player_aimbot_info[ entity->index( ) ];
		}

	private:
		bool DoHitscan( sdk::base_entity* entity, sdk::vec3& final_position, int& final_damage );
		bool DoHeadAim( sdk::base_entity* entity, sdk::vec3& final_position, int& final_damage );

		bool IsAccurate( sdk::base_entity* entity, sdk::vec3 position );

		/// helper functions
		void GetMultipointPositions( sdk::base_entity* entity, std::vector<sdk::vec3>& positions, int hitbox_index, float pointscale );
		static sdk::mstudiobbox_t* GetHitbox( sdk::base_entity* entity, int hitbox_index );
	};

	extern automatic_shoot_at_target_bot aimbot;
}
