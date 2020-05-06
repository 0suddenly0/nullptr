#pragma once

#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve 9107
#define k_EMsgGCClientWelcome 4004
#define k_EMsgGCClientHello 4006
#define k_EMsgGCAdjustItemEquippedState 1059
#define k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello 9109
#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello 9110
#define k_EMsgGCCStrike15_v2_ClientGCRankUpdate  9194

#include <stdint.h>
#include "pbwrap.h"

enum rank_type : int
{
    mm = 6,
    wingmans = 7,
    danger_zone = 10
};

enum mode_settigns : int
{
    matchmaking = 0,
    wingman = 1,
    dangerzone = 2
};

// ProfileChanger

struct _MatchmakingGC2ClientHello {
	static const uint32_t commendation = 8;
	static const uint32_t ranking = 7;
	static const uint32_t player_level = 17;
	static const uint32_t penalty_seconds = 4;
	static const uint32_t penalty_reason = 5;
	static const uint32_t player_cur_xp = 18;
	static const uint32_t rank_type_id = 6;
};
struct _PlayerCommendationInfo {
	static const uint32_t cmd_friendly = 1;
	static const uint32_t cmd_teaching = 2;
	static const uint32_t cmd_leader = 4;
};
struct _PlayerRankingInfo 
{
	static const uint32_t account_id = 1;
	static const uint32_t rank_id = 2;
	static const uint32_t wins = 3;
    //static const uint32_t game_type = 6;
};

#pragma once
#include "pbwrap.h"

#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve 9107
#define k_EMsgGCClientWelcome 4004
#define k_EMsgGCClientHello 4006
#define k_EMsgGCAdjustItemEquippedState 1059
#define k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello 9109
#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello 9110
#define k_EMsgGCCStrike15_v2_ClientGCRankUpdate  9194

//FORMAT: constexpr static Tag field_name = {field_id, field_type};

using namespace pbwrap;

struct CMsgClientHello : pbmsg<8> {
    PBMSG_CTOR;
    PBFIELD(3, types::Uint32, client_session_need);
};

// ProfileChanger
struct MatchmakingGC2ClientHello : pbmsg<20> {
    struct PlayerRankingInfo : pbmsg<6> {
        PBMSG_CTOR;
        PBFIELD(1, types::Uint32, account_id);
        PBFIELD(2, types::Uint32, rank_id);
        PBFIELD(3, types::Uint32, wins);
        PBFIELD(6, types::Uint32, rank_type_id);
    };
    struct PlayerCommendationInfo : pbmsg<4> {
        PBMSG_CTOR;
        PBFIELD(1, types::Uint32, cmd_friendly);
        PBFIELD(2, types::Uint32, cmd_teaching);
        PBFIELD(4, types::Uint32, cmd_leader);
    };

    PBMSG_CTOR;
    PBFIELD(7, PlayerRankingInfo, ranking);
    PBFIELD(8, PlayerCommendationInfo, commendation);
    PBFIELD(17, types::Int32, player_level);
    PBFIELD(18, types::Int32, player_level_xp);
};

struct CMsgGCCStrike15_v2_ClientGCRankUpdate : pbmsg<1> {
    PBMSG_CTOR;
    PBFIELD(1, MatchmakingGC2ClientHello::PlayerRankingInfo, ranking);
};

