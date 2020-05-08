#pragma once
#include "../imgui/imgui.h"
#include "../valve_sdk/math/Vector.h"
#include "../valve_sdk/Misc/Color.h"
#include "../valve_sdk/csgostructs.h"
#include "globals.h"
//#include "../features/skin_changer/kit_parser.h"
//#include "../features/skin_changer/item_definitions.h"

#include <string>
#include <map>
#include <vector>
#include <array>

#define NULLPTR_VERSION 0.1

#define FLOAT_TO_CLR(clr) clr[0] * 255.0f, clr[1] * 255.0f, clr[2] * 255.0f, clr[3] * 255.0f

extern std::map<int, std::string> weapon_names;
extern std::map<int, std::string> weapon_groups;
extern std::map<int, std::string> mode_names;
extern std::map<int, std::string> entity_names;
extern std::map<int, std::string> hitbox_names;
extern std::map<int, std::string> entity_names_ragdoll;

struct event_settings_t
{
	bool enable_screen;
	bool enable_console;
	
	bool custom_color;
	Color color;
};

struct item_setting
{
	char name[32] = "Default";
	bool enabled = false;
	int stickers_place = 0;
	int definition_vector_index = 0;
	int definition_index = 0;
	bool   enabled_stickers = 0;
	int paint_kit_vector_index = 0;
	int paint_kit_index = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	bool stat_trak = 0;
	float wear = FLT_MIN;
	char custom_name[32] = "";
};

struct statrack_setting
{
	int definition_index = 1;
	struct
	{
		int counter = 0;
	}statrack_new;
};

struct legitbot_settings 
{
	bool enabled = false;

	bool enable_hitbox_override = false;
	KeyBind_t hitbox_override_bind = { 0, 0, false };
	bool override_hitboxes[7] = {false, false, false, false, false, false, false};

	bool hitboxes[7] = { false, false, false, false, false, false, false };

	bool smoke_check = false;
	bool flash_check = false;
	int flash_check_min = 100;
	bool jump_check = false;
	bool only_in_zoom = true;

	bool deathmatch = false;
	bool autopistol = false;
	bool autowall = false;
	bool aim_at_backtrack = true;
	int backtrack_time = 0;
	int smooth_type = 0;
	float fov = 0.f;
	int silent_chance = 100;
	float silent_fov = 0.f;
	float smooth = 1;
	int kill_delay = 0;

	int rcs_type = 0;
	bool rcs = false;
	bool rcs_fov_enabled = false;
	bool rcs_smooth_enabled = false;
	float rcs_fov = 0.f;
	float rcs_smooth = 1;
	int rcs_x = 100;
	int rcs_y = 100;
	int rcs_start = 1;

	int min_damage = 1;
};

struct profilechanger_settings
{
	int wins = 0;
	int rank_id = 0;
};

enum esp_types : int
{
	enemies = 0,
	teammates = 1,
	local_player = 2
};

struct glow_settings
{
	bool enable = false;
	bool visible_only = false;

	Color visible = { 0, 0, 0, 255 };
	Color in_visible = { 0, 0, 0, 255 };
};

struct esp_settings
{
	bool enable = false;
	bool only_visible = false;
	
	bool skeleton = false;
	bool box = false;
	bool name = false;

	bool health_bar = false;
	bool health_in_bar = false;
	bool health_based = false;
	int health_position = 0;

	bool armor_bar = false;
	bool armor_in_bar = false;
	int armor_position = 1;

	bool weapon = false;
	bool weapon_ammo = false;

	bool flags_scoped = false;
	bool flags_flashed = false;
	bool flags_defusing = false;
	bool flags_planting = false;
	bool flags_bomb_carrier = false;
	bool flags_armor = false;
	bool flags_helmet = false;

	Color dormant = Color(0, 0, 0, 255);

	Color box_visible = Color(0, 0, 0, 255);
	Color box_invisible = Color(0, 0, 0, 255);

	Color skeleton_visible = Color(0, 0, 0, 255);
	Color skeleton_invisible = Color(0, 0, 0, 255);

	Color health_bar_outline = Color(0, 0, 0, 255);
	Color health_bar_background = Color(0, 0, 0, 255);
	Color health_bar_main = Color(0, 0, 0, 255);

	Color armor_bar_outline = Color(0, 0, 0, 255);
	Color armor_bar_background = Color(0, 0, 0, 255);
	Color armor_bar_main = Color(0, 0, 0, 255);

	Color ammo_bar_outline = Color(0, 0, 0, 255);
	Color ammo_bar_background = Color(0, 0, 0, 255);
	Color ammo_bar_main = Color(0, 0, 0, 255);
};

struct chams_settings
{
	bool enable = false;
	bool only_visible = false;

	int chams_type = 0;

	Color visible = Color(0, 0, 0, 255);
	Color invisible = Color(0, 0, 0, 255);
};

struct chams_settings_t
{
	chams_settings top;
	chams_settings bot;
};


namespace settings
{	
	namespace windows
	{
		ImVec2 Bind_pos = ImVec2(100, 200);
		float Bind_alpha = 1.f;

		ImVec2 Spec_pos = ImVec2(100, 100);
		float Spec_alpha = 1.f;
	}

	namespace legit_bot
	{
		bool auto_current = false;
		int settings_category = 0;
		int weapon_group = 0;
		int settings_weapon = 0;

		//legitbot_settings                legitbot_items_all;
		//std::array<legitbot_settings, 100> legitbot_items;
		//std::array<legitbot_settings, 7> legitbot_items_groups;

		legitbot_settings                legitbot_items_all;
		extern std::map<int, legitbot_settings> legitbot_items;
		extern std::map<int, legitbot_settings> legitbot_items_groups;

		KeyBind_t bind = { 0, 0, false };
		bool bind_check = false;
	}

	namespace visuals
	{
		bool spectator_list = false;
		bool radar_ingame = false;
		bool bomb_timer = false;

		int outline_type = 0;

		namespace hitbox
		{
			bool enable = false;
			Color color = Color(0, 0, 0, 255);
			float show_time = 1.f;
			int show_type = 0;
		}
		namespace events
		{
			bool enable = true;

			event_settings_t hurt = event_settings_t{true, true, false, Color(255,255,255,255)};
			event_settings_t player_buy = event_settings_t{ false, false, false, Color(255,255,255,255)};
			event_settings_t planting = event_settings_t{ true, true, false, Color(255,255,255,255)};
			event_settings_t defusing = event_settings_t{ true, true, false, Color(255,255,255,255)};
			event_settings_t config = event_settings_t{ true, true, false, Color(255,255,255,255)};
			event_settings_t lua = event_settings_t{ true, true, false, Color(255,255,255,255) };
		}

		namespace bullet_impact
		{
			namespace box
			{
				bool enable_c;
				Color color_c = Color(0, 0, 0, 255);
				float time_c = 2.f;
				float size_c = 2.f;

				bool enable_s;
				Color color_s = Color(0, 0, 0, 255);
				float time_s = 2.f;
				float size_s = 2.f;
			}

			namespace line
			{
				bool enable = false;
				bool local_player = false;
				bool teammates = false;
				bool enemies = false;

				Color color_local = Color(0, 0, 0, 255);
				Color color_teammates = Color(0, 0, 0, 255);
				Color color_enemies = Color(0, 0, 0, 255);
				float time = 2.f;
			}
		}

		namespace damage_indicator
		{
			bool enable = false;

			float show_time = 5.f;
			float speed = 0.2f;
			float max_pos_y = 20.f;
			float range_offset_x = 10.f;
			float offset_if_kill = 10.f;
			float offset_hit = 0.f;

			float text_size = 12.f;
			Color standart_color = Color(255, 255, 255, 255);
			Color kill_color = Color(255, 50, 50, 255);
		}

		namespace grenade_prediction
		{
			bool enable = false;
			bool radius = true;

			float line_thickness = 2.f;
			float colision_box_size = 2.f;
			float main_colision_box_size = 3.f;

			Color radius_color = Color(0, 0, 0, 255);
			Color main = Color(0, 0, 0, 255);
			Color main_box = Color(0, 0, 0, 255);
			Color end_box = Color(0, 0, 0, 255);
		}
		
		namespace grenades
		{
			bool enable = false;

			bool smoke_timer = false;
			bool smoke_bar = true;
			bool smoke_radius = true;
			bool molotov_timer = false;
			bool molotov_bar = true;
			bool molotov_radius = false;

			Color color_smoke = Color(255, 255, 255, 255);
			Color color_molotov = Color(255, 255, 255, 255);
			Color color_decoy = Color(255, 255, 255, 255);
			Color color_flash = Color(255, 255, 255, 255);
			Color color_frag = Color(255, 255, 255, 255);

			Color color_bar_smoke_back = Color(20, 20, 20, 100);
			Color color_bar_smoke_main = Color(100, 100, 255, 255);
			Color color_smoke_radius = Color(100, 100, 255, 255);

			Color color_bar_molotov_back = Color(20, 20, 20, 100);
			Color color_bar_molotov_main = Color(255, 100, 100, 255);
			Color color_molotov_radius = Color(100, 100, 255, 255);
		}

		namespace dropped_weapon
		{
			bool enable = false;
			bool box = false;
			bool ammo_bar = false;

			Color box_color = Color(0, 0, 0, 255);

			Color bar_outline = Color(0, 0, 0, 255);
			Color bar_background = Color(0, 0, 0, 255);
			Color bar_main = Color(0, 0, 0, 255);
		}

		namespace ofc
		{
			bool enable = false;
			bool visible_check = false;

			float range = 26.f;
			float hight = 6.f;
			float wight = 12.f;

			Color color = Color(255, 255, 255, 255);
		}

		namespace fov
		{
			bool fov_basic = false;
			bool fov_basic_background = false;
			bool fov_silent = false;
			bool fov_silent_background = false;

			Color color_fov_basic = Color(0, 0, 0, 255);
			Color color_fov_basic_background = Color(0, 0, 0, 255);
			Color color_fov_silent = Color(0, 0, 0, 255);
			Color color_fov_silent_background = Color(0, 0, 0, 255);
		}

		namespace esp
		{
			std::array<esp_settings, 3> esp_items;
		}

		namespace glow
		{
			std::array<glow_settings, 3> glow_items;
		}

		namespace chams
		{
			int tick_draw = 0;
			chams_settings_t backtrack_settings;
			chams_settings_t hands_settings;
			chams_settings_t weapon_settings;
			chams_settings_t weapon_dropped_settings;

			std::array<chams_settings_t, 3> player_settings;
			std::array<chams_settings_t, 2> ragdoll_settings;
		}
	}

	namespace misc
	{
		Color menu_color = Color(100, 100, 255);

		int model_index = 0;

		bool auto_accept = false;
		bool choke_indicator = false;
		bool hitmarker = false;
		bool reveal_money = false;
		bool reveal_rank = false;
		bool nightmode = false;
		bool moon_walk = false;
		bool fast_duck = false;
		bool watermark = true;

		bool disable_flash = false;
		int flash_alpha = 50;
		bool disable_smoke = false;
		bool disable_zoom = false;
		bool disable_zoom_border = false;
		bool sniper_crosshair = false;

		namespace prepare_revolver
		{
			bool enable = false;
			KeyBind_t bind = { 0, 0, false };
		}

		namespace fog
		{
			bool enable = false;
			Color color = Color(50, 50, 50, 120);
			float start_dist = 20.f;
			float end_dist = 100.f;
		}

		namespace inverse_gravity
		{
			bool enable_slow = true;
			bool enable = false;
			int value = 4;
		}

		namespace hitsound
		{
			bool enable = false;
			float volume = 1.f;
		}

		namespace bhop
		{
			bool enable = false;
			bool auto_strafer = false;
			int strafer_type = 1;
			float retrack_speed = 0.f;
		}

		namespace edge_jump
		{
			bool enable = false;
			bool auto_duck = false;
			KeyBind_t bind = { 0, 0, false};
		}

		namespace clantag
		{
			bool enable = false;
			bool check_empty = true;

			std::string clantag = "nullptr ";
			std::string clantag_visible = "nullptr ";

			int clantag_type = 0;
			int animation_type = 0;
			int custom_type = 0;
			float speed = 0.4f;
		}

		namespace viewmodel
		{
			float override = 90;
			float viewmodel = 68;
			float fov_x = 0;
			float fov_y = 0;
			float fov_z = 0;
		}

		namespace fake_latency
		{
			bool enable = false;
			int amount = 0;
		}

		namespace third_person
		{
			bool enable = false;
			KeyBind_t bind = { 0, 0, false };
			float dist = 50.f;
		}

		namespace bind_window
		{
			bool enable;
		}

		namespace spreed_circle
		{
			bool enable = false;
			Color color = { 255, 255, 255, 50 };
		}

		namespace desync
		{			
			namespace indicator
			{
				bool arrow = true;
				Color real = Color(255, 255, 255, 255);
				Color fake = Color(255, 0, 0, 255);
				float size_x = 20.f;
				float size_y = 5.f;
				float offset_x = 100.f;

				bool lines = false;
			}

			int type = 0;
			bool auto_der = false;
			KeyBind_t bind = { 0, 0, false };
			float side = 1.f;
		}

		namespace slowwalk
		{
			bool enable = false;
			float speed = 0.f;
			KeyBind_t bind = { 0, 0, false };
		}

		namespace block_bot
		{
			bool enable = false;
			KeyBind_t bind = { 0, 0, false };
		}

		namespace fake_lag
		{
			bool enable = false;

			bool lag_when_stand = false;
			bool lag_when_move = false;

			int type = 0;
			int factor = 0;
		}
	}

	namespace changers
	{
		namespace profile
		{
			//std::array<profilechanger_settings, 3> profile_items;
			extern std::map<int, profilechanger_settings> profile_items;

			int mode_id;
			int private_id = 0;
			int private_xp = 0;
			int teacher = 0;
			int leader = 0;
			int friendly = 0;
			int ban_type = 0;
			int ban_time = 0;
			int ban_time_type = 0;
		}

		namespace skin
		{
			bool skin_preview = false;
			bool show_cur = true;

			extern std::map<int, statrack_setting> statrack_items;
			extern std::map<int, item_setting> m_items;
			extern std::map<std::string, std::string> m_icon_overrides;
		}
	}
}

static char* chams_types[] = {
"regular",
"flat",
"glow",
"warrame"
};

static char* tabs_chams[] = { "players", "world" };
static char* tabs_c[] = { "legit", "visual", "misc", "skin", "profile", "config", "lua"};
static char* esp_tabs_general[] = {"esp", "glow", "chams", "other", "world"};
static char* lua_tabs_general[] = { "lua list", "function"};
static char* players_tabs[] = { "enemies", "teammates", "local player" };
static char* layer_tabs[] = { "bottom layer", "top layer" };
static char* players_tabs_ragdoll[] = { "enemies", "teammates" };
static char* chams_types_model[] = { "standart", "ragdoll" };
extern bool  g_unload;