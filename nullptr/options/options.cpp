#include "options.h"
#include "../valve_sdk/Misc/Enums.h"

std::map<int, std::string> mode_names =
{
	{0,"matchmaking"},
	{1,"wingman"},
	{2,"dangerzone"}
};

std::map<int, std::string> entity_names =
{
	{0,"enemy"},
	{1,"teammates"},
	{2,"local player"}
};

std::map<int, std::string> entity_names_ragdoll =
{
	{0,"enemy"},
	{1,"teammates"},
};

std::map<int, std::string> hitbox_names =
{
	{0, "head"},
	{1, "neck"},
	{2, "pelvis"},
	{3, "stomach"},
	{4, "lower chest"},
	{5, "chest"},
	{6, "upper chest"}
};

std::map<int, std::string> weapon_names =
{
	{ 228, "knife" },
{ 31, "zeus" },
{ 7, "ak-47" },
{ 8, "aug" },
{ 9, "awp" },
{ 63, "cz75 auto" },
{ 1, "desert eagle" },
{ 2, "dual berettas" },
{ 10, "famas" },
{ 3, "five-seven" },
{ 11, "g3sg1" },
{ 13, "galil ar" },
{ 4, "glock-18" },
{ 14, "m249" },
{ 60, "m4a1-s" },
{ 16, "m4a4" },
{ 17, "mac-10" },
{ 27, "mag-7" },
{ 33, "mp7" },
{ WEAPON_MP5, "mp5" },
{ 34, "mp9" },
{ 28, "negev" },
{ 35, "nova" },
{ 32, "p2000" },
{ 36, "p250" },
{ 19, "p90" },
{ 26, "pp-bizon" },
{ 64, "r8 revolver" },
{ 29, "sawed-off" },
{ 38, "scar-20" },
{ 40, "ssg 08" },
{ 39, "sg 553" },
{ 30, "tec-9" },
{ 24, "ump-45" },
{ 61, "usp-s" },
{ 25, "xm1014" },
};

std::map<int, std::string> weapon_groups =
{
{0, "knife"},
{1, "zeus"},
{2, "pistols"},
{3, "semi-rifle"},
{4, "shot gun"},
{5, "machin gun"},
{6, "rifle"},
{7, "sniper rifle"}
};

namespace settings
{
	namespace legit_bot
	{
		std::map<int, legitbot_settings> legitbot_items = { };
		std::map<int, legitbot_settings> legitbot_items_groups = { };
	}

	namespace changers
	{
		namespace profile
		{
			std::map<int, profilechanger_settings> profile_items = { };
		}
		namespace skin 
		{
			std::map<int, statrack_setting> statrack_items = { };
			std::map<int, item_setting> m_items = { };
			std::map<std::string, std::string> m_icon_overrides = { };
		}
	}
}

bool g_unload = false;