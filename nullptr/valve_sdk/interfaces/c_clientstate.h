#pragma once

#include <cstdint>

// Created with ReClass.NET by KN4CK3R
#pragma pack(push, 1)
class i_net_channel
{
public:
    char pad_0000[20];           //0x0000
    bool processing_messages;  //0x0014
    bool should_delete;        //0x0015
    char pad_0016[2];            //0x0016
    int32_t out_sequence_nr;    //0x0018 last send outgoing sequence number
    int32_t in_sequence_nr;     //0x001C last received incoming sequnec number
    int32_t out_sequence_nr_ack; //0x0020 last received acknowledge outgoing sequnce number
    int32_t out_reliable_state; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
    int32_t in_reliable_state;  //0x0028 state of incoming reliable data
    int32_t choked_packets;    //0x002C number of choked packets
    char pad_0030[1044];         //0x0030
}; //Size: 0x0444

class c_clock_drift_mgr
{
public:
    float clock_offsets[16];   //0x0000
    uint32_t cur_clock_offset; //0x0044
    uint32_t server_tick;     //0x0048
    uint32_t client_tick;     //0x004C
}; //Size: 0x0050

class c_clientstate {
public:
	void force_full_update() {
		*reinterpret_cast<int*>(std::uintptr_t(this) + 0x174) = -1;
	}

	char pad_0000[156];             //0x0000
	i_net_channel* net_channel;      //0x009C
	uint32_t challenge_nr;        //0x00A0
	char pad_00A4[100];             //0x00A4
	uint32_t signon_state;        //0x0108
	char pad_010C[8];               //0x010C
	float next_cmd_time;          //0x0114
	uint32_t server_count;        //0x0118
	uint32_t current_sequence;    //0x011C
	char pad_0120[8];               //0x0120
	c_clock_drift_mgr clock_drift_mgr; //0x0128
	uint32_t delta_tick;          //0x0174
	bool paused;                 //0x0178
	char pad_017D[3];               //0x017D
	uint32_t view_entity;         //0x0180
	uint32_t player_slot;         //0x0184
	char level_name[260];        //0x0188
	char level_name_short[80];    //0x028C
	char group_name[80];         //0x02DC
	char pad_032Ñ[92];              //0x032Ñ
	uint32_t max_clients;         //0x0388
	char pad_0314[18824];           //0x0314
	float last_server_tick_time;   //0x4C98
	bool in_simulation;              //0x4C9C
	char pad_4C9D[3];               //0x4C9D
	uint32_t old_tick_count;          //0x4CA0
	float tick_remainder;          //0x4CA4
	float frame_time;              //0x4CA8
	int last_out_going_command;        //0x4CAC
	int choked_commands;             //0x4CB0
	int last_command_ack;           //0x4CB4
	int command_ack;                //0x4CB8
	int sound_sequence;           //0x4CBC
	char pad_4CC0[80];              //0x4CC0
	QAngle viewangles;              //0x4D10
	char pad_4D1C[208];             //0x4D1C
}; //Size: 0x4D1C

#pragma pack(pop)

static_assert( FIELD_OFFSET(c_clientstate, net_channel ) == 0x009C, "Wrong struct offset" );
static_assert( FIELD_OFFSET(c_clientstate, current_sequence ) == 0x011C, "Wrong struct offset" );
static_assert( FIELD_OFFSET(c_clientstate, delta_tick ) == 0x0174, "Wrong struct offset" );
