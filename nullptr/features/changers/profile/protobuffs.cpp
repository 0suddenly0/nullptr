#include "protobuffs.h"
#include "pbwrap.h"
#include "../../../options/options.h"
#include "protoparser.h"
#include "profile_info_changer.h"
#include "../../../valve_sdk/sdk.h"
#include "proto_writer.h"


#define CAST(cast, address, add) reinterpret_cast<cast>((uint32_t)address + (uint32_t)add)

void profile_protobuff::write_packet(std::string packet, void* thisPtr, void* oldEBP, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
{
	if ((uint32_t)packet.size() <= cubDest - 8)
	{
		memcpy((void*)((DWORD)pubDest + 8), (void*)packet.data(), packet.size());
		*pcubMsgSize = packet.size() + 8;
	}
	else if (g_mem_alloc)
	{
		auto memPtr = *CAST(void**, thisPtr, 0x14);
		auto memPtrSize = *CAST(uint32_t*, thisPtr, 0x18);
		auto newSize = (memPtrSize - cubDest) + packet.size() + 8;

		auto memory = g_mem_alloc->realloc(memPtr, newSize + 4);

		*CAST(void**, thisPtr, 0x14) = memory;
		*CAST(uint32_t*, thisPtr, 0x18) = newSize;
		*CAST(void**, oldEBP, -0x14) = memory;

		memcpy(CAST(void*, memory, 24), (void*)packet.data(), packet.size());

		*pcubMsgSize = packet.size() + 8;
	}
}
void profile_protobuff::receive_message(void* thisPtr, void* oldEBP, uint32_t messageType, void* pubDest, uint32_t cubDest, uint32_t * pcubMsgSize)
{
	static bool standart_upd;
	static bool oth_upd;

	if (messageType == k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello)
	{
		auto packet = profile_info_changer(pubDest, pcubMsgSize);
		write_packet(packet, thisPtr, oldEBP, pubDest, cubDest, pcubMsgSize);
		standart_upd = true;
	}
	if (messageType == k_EMsgGCCStrike15_v2_ClientGCRankUpdate)
	{
		CMsgGCCStrike15_v2_ClientGCRankUpdate msg((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8);

		auto ranking = msg.ranking().get_all();

		for (int i = 0; i < ranking.size(); i++)
		{
			auto Ranking = ranking[i];

			if (Ranking.rank_type_id().get() == rank_type::wingmans)
			{
				Ranking.rank_id().set(settings::changers::profile::profile_items[mode_settigns::wingman].rank_id);
				Ranking.wins().set(settings::changers::profile::profile_items[mode_settigns::wingman].wins);

				msg.ranking().set(Ranking, i);
			}
			if (Ranking.rank_type_id().get() == rank_type::danger_zone)
			{
				Ranking.rank_id().set(settings::changers::profile::profile_items[mode_settigns::dangerzone].rank_id);
				Ranking.wins().set(settings::changers::profile::profile_items[mode_settigns::dangerzone].wins);

				msg.ranking().set(Ranking, i);
			}
		}

		auto packet = msg.serialize();

		write_packet(packet, thisPtr, oldEBP, pubDest, cubDest, pcubMsgSize);

		oth_upd = true;
	}

	if (oth_upd && standart_upd)
	{
		oth_upd = false;
		standart_upd = false;
		globals::profile::update_request = false;
	}
}

///////////////////////////////////
/******** Custom Messages ********/
///////////////////////////////////
bool profile_protobuff::send_client_hello()
{
	CMsgClientHello msg;
	msg.client_session_need().set(1);
	auto packet = msg.serialize();

	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_game_coordinator->gc_send_message(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}

bool profile_protobuff::send_matchmaking_client_2gc_hello()
{
	ProtoWriter msg(0);

	auto packet = msg.serialize();
	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_game_coordinator->gc_send_message(k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}

bool profile_protobuff::send_client_gc_rank_update()
{
	MatchmakingGC2ClientHello::PlayerRankingInfo rank_wingman;
	MatchmakingGC2ClientHello::PlayerRankingInfo rank_danger;
	rank_wingman.rank_type_id().set(7); // 6 - mm, 7 - wingman
	rank_danger.rank_type_id().set(10); // 6 - mm, 7 - wingman

	CMsgGCCStrike15_v2_ClientGCRankUpdate msg;
	msg.ranking().add(rank_wingman);
	msg.ranking().add(rank_danger);

	auto packet = msg.serialize();

	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_game_coordinator->gc_send_message(k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}
