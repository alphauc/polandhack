#pragma once
#include "../misc/vector.hh"
#include <cstdint>

namespace sdk {
	class clock_drift_manager {
	public:
		float clock_offsets[17];   //0x0000
		uint32_t cur_clock_offset; //0x0044
		uint32_t server_tick;     //0x0048
		uint32_t client_tick;     //0x004C
	}; //Size: 0x0050

	class net_channel {
	public:
		char pad_0x0000[0x18]; //0x0000
		__int32 out_sequence_nr; //0x0018 
		__int32 in_sequence_nr; //0x001C 
		__int32 out_sequence_nr_ack; //0x0020 
		__int32 out_reliable_state; //0x0024 
		__int32 in_reliable_state; //0x0028 
		__int32 choked_packets; //0x002C 

	};

	class client_state {
	public:
		void full_update() {
			*reinterpret_cast<int*>(uintptr_t(this) + 0x174) = -1;
		};

		char pad_0000[156];             //0x0000
		net_channel* net_channel;          //0x009C
		uint32_t challenge_nr;        //0x00A0
		char pad_00A4[100];             //0x00A4
		uint32_t signon_state;        //0x0108
		char pad_010C[8];               //0x010C
		float flNextCmdTime;          //0x0114
		uint32_t server_count;        //0x0118
		uint32_t current_sequence;    //0x011C
		char pad_0120[8];               //0x0120
		clock_drift_manager clock_drift_mgr; //0x0128
		uint32_t delta_tick;          //0x0178
		bool paused;                 //0x017C
		char pad_017D[3];               //0x017D
		uint32_t nViewEntity;         //0x0180
		uint32_t nPlayerSlot;         //0x0184
		char szLevelName[260];        //0x0188
		char szLevelNameShort[40];    //0x028C
		char szGroupName[40];         //0x02B4
		char pad_02DC[52];              //0x02DC
		uint32_t max_clients;         //0x0310
		char pad_0314[18820];           //0x0314
		float flLastServerTickTime;   //0x4C98
		bool insimulation;              //0x4C9C
		char pad_4C9D[3];               //0x4C9D
		uint32_t oldtickcount;          //0x4CA0
		float tickRemainder;          //0x4CA4
		float frameTime;              //0x4CA8
		uint32_t nLastOutgoingCommand;   //0x4CAC
		uint32_t choked_commands;        //0x4CB0
		uint32_t nLastCommandAck;      //0x4CB4
		uint32_t nCommandAck;           //0x4CB8
		uint32_t nSoundSequence;      //0x4CBC
		char pad_4CC0[80];              //0x4CC0
		vec3 view_angles;              //0x4D10
	}; //Size: 0x4D1C

}