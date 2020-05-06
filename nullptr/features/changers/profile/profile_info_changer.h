#pragma once
//#include "../../pbwrap.hpp"
#include "protobuff_messages.h"
#include "../../../options/options.h"
#include "proto_field.h"
#include "proto_writer.h"

#define _gc2ch _MatchmakingGC2ClientHello
#define _pci _PlayerCommendationInfo
#define _pri _PlayerRankingInfo

static std::string profile_info_changer(void* pubDest, uint32_t* pcubMsgSize)
{

	c_proto_writer msg((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8, 30);

	//replace commends
	auto _commendation = msg.has(_gc2ch::commendation) ? msg.get(_gc2ch::commendation).String() : std::string("");
	c_proto_writer commendation(_commendation, 4);

	if (settings::changers::profile::friendly >= 1)
		commendation.replace(c_field(_pci::cmd_friendly, _TYPE_UINT32, (int64_t)settings::changers::profile::friendly));

	if (settings::changers::profile::teacher >= 1)
		commendation.replace(c_field(_pci::cmd_teaching, _TYPE_UINT32, (int64_t)settings::changers::profile::teacher));

	if (settings::changers::profile::leader >= 1)
		commendation.replace(c_field(_pci::cmd_leader, _TYPE_UINT32, (int64_t)settings::changers::profile::leader));

	msg.replace(c_field(_gc2ch::commendation, _TYPE_STRING, commendation.serialize()));


	auto _ranking = msg.has(_gc2ch::ranking) ? msg.get(_gc2ch::ranking).String() : std::string("");
	c_proto_writer ranking(_ranking, 7);

	//ranking.replace(c_field(_pri::game_type, _TYPE_UINT32, (int64_t)10));
	ranking.replace(c_field(_pri::rank_id, _TYPE_UINT32, (int64_t)settings::changers::profile::profile_items[mode_settigns::matchmaking].rank_id));
	ranking.replace(c_field(_pri::wins, _TYPE_UINT32, (int64_t)settings::changers::profile::profile_items[mode_settigns::matchmaking].wins));
	msg.replace(c_field(_gc2ch::ranking, _TYPE_STRING, ranking.serialize()));


	//replace private level
	msg.replace(c_field(_gc2ch::player_level, _TYPE_INT32, (int64_t)settings::changers::profile::private_id + 1));
	msg.replace(c_field(_gc2ch::player_cur_xp, _TYPE_INT32, (int64_t)settings::changers::profile::private_xp));
	//bans
	if (settings::changers::profile::ban_type != 0 && settings::changers::profile::ban_time != 0)
	{
		msg.replace(c_field(_gc2ch::penalty_reason, _TYPE_INT32, (int64_t)settings::changers::profile::ban_type));
		static int num;
		if (settings::changers::profile::ban_time_type == 0)
		{
			num = settings::changers::profile::ban_time;
		}
		else if (settings::changers::profile::ban_time_type == 1)
		{
			num = settings::changers::profile::ban_time * 60;
		}
		else if (settings::changers::profile::ban_time_type == 2)
		{
			num = settings::changers::profile::ban_time * 3600;
		}
		else if (settings::changers::profile::ban_time_type == 3)
		{
			num = settings::changers::profile::ban_time * 86400;
		}
		msg.replace(c_field(_gc2ch::penalty_seconds, _TYPE_INT32, (int64_t)num));
	}
	return msg.serialize();
}