#pragma once
#include <windows.h>
#include <string>


class profile_protobuff
{
public:
	static void write_packet(std::string packet, void* thisPtr, void* oldEBP, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
	void receive_message(void* thisPtr, void* oldEBP, uint32_t messageType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
	bool send_client_hello();
	bool send_matchmaking_client_2gc_hello();
	bool send_client_gc_rank_update();
};