#include "notify.h"
#include "../../options/options.h"
#include <map>

std::map<std::string, std::string> event_to_normal =
{
	//others
	////////////////////////////////////////////////
	{ "weapon_taser", "zeus" },
	{ "item_kevlar", "kevlar" },
	{ "item_defuser", "defuse kit" },
	{ "item_assaultsuit", "full armor" },
	////////////////////////////////////////////////
	//


	//pistols
	////////////////////////////////////////////////
	{ "weapon_p250", "p250" },
	{ "weapon_tec9", "tec-9" },
	{ "weapon_cz75a", "cz75a" },
	{ "weapon_glock", "glock" },
	{ "weapon_elite", "elite" },
	{ "weapon_deagle", "deagle" },
	{ "weapon_hkp2000", "p2000" },
	{ "weapon_usp_silencer", "usp" },
	{ "weapon_revolver", "rovelver" },
	{ "weapon_fiveseven", "five-seven" },
	////////////////////////////////////////////////
	//

	//pp
	////////////////////////////////////////////////
	{ "weapon_mp9", "mp-9" },
	{ "weapon_mac10", "mac-10" },
	{ "weapon_mp7", "mp-7" },
	{ "weapon_mp5sd", "mp5-sd" },
	{ "weapon_ump45", "ump-45" },
	{ "weapon_p90", "p90" },
	{ "weapon_bizon", "bizon" },
	////////////////////////////////////////////////
	//

	//rifles
	////////////////////////////////////////////////
	{ "weapon_famas", "famas" },
	{ "weapon_m4a1_silencer", "m4a1-s" },
	{ "weapon_m4a1", "m4a1" },
	{ "weapon_ssg08", "ssg08" },
	{ "weapon_aug", "aug" },
	{ "weapon_awp", "awp" },
	{ "weapon_scar20", "scar20" },
	{ "weapon_galilar", "galil" },
	{ "weapon_ak47", "ak 47" },
	{ "weapon_sg556", "sg553" },
	{ "weapon_g3sg1", "g3sg1" },
	////////////////////////////////////////////////
	//

	//have
	////////////////////////////////////////////////
	{ "weapon_nova", "nova" },
	{ "weapon_xm1014", "xm1014" },
	{ "weapon_sawedoff", "sawedoff" },
	{ "weapon_m249", "m249" },
	{ "weapon_negev", "negev" },
	{ "weapon_mag7", "mag7" },
	////////////////////////////////////////////////
	//

	//granades
	////////////////////////////////////////////////
	{ "weapon_flashbang", "flash" },
	{ "weapon_smokegrenade", "smoke" },
	{ "weapon_molotov", "molotov" },
	{ "weapon_incgrenade", "inc" },
	{ "weapon_decoy", "decoy" },
	{ "weapon_hegrenade", "hae" },
	////////////////////////////////////////////////
	//
};

std::string bomb_site(int site_id, std::string map_name)
{
	if (map_name == "de_inferno")
	{
		if (site_id == 333)
		{
			return "A";
		}
		else if (site_id == 422)
		{
			return "B";
		}
	}
	else if (map_name == "de_mirage")
	{
		if (site_id == 425)
		{
			return "A";
		}
		else if (site_id == 426)
		{
			return "B";
		}
	}
	else if (map_name == "de_dust2")
	{
		if (site_id == 281)
		{
			return "A";
		}
		else if (site_id == 282)
		{
			return "B";
		}
	}
	else if (map_name == "de_overpass")
	{
		if (site_id == 79)
		{
			return "A";
		}
		else if (site_id == 504)
		{
			return "B";
		}
	}
	else if (map_name == "de_vertigo")
	{
		if (site_id == 262)
		{
			return "A";
		}
		else if (site_id == 314)
		{
			return "B";
		}
	}
	else if (map_name == "de_nuke")
	{
		if (site_id == 154)
		{
			return "A";
		}
		else if (site_id == 167)
		{
			return "B";
		}
	}
	else if (map_name == "de_train")
	{
		if (site_id == 94)
		{
			return "A";
		}
		else if (site_id == 536)
		{
			return "B";
		}
	}
	else if (map_name == "de_cache")
	{
		if (site_id == 317)
		{
			return "A";
		}
		else if (site_id == 318)
		{
			return "B";
		}
	}
	else if (map_name == "de_cbble")
	{
		if (site_id == 216)
		{
			return "A";
		}
		else if (site_id == 107)
		{
			return "B";
		}
	}
	else if (map_name == "de_shortdust")
	{
		if (site_id == 217)
		{
			return "A";
		}
	}
	else if (map_name == "de_rialto")
	{
		if (site_id == 99)
		{
			return "A";
		}
	}
	else if (map_name == "de_lake")
	{
		if (site_id == 209)
		{
			return "A";
		}
	}

	return "unck";

}

struct notify_t
{
	std::string s_pre;
	std::string s_body;
	Color c_pre;
	Color c_body;

	float time_del;
	float time_life = 0;

	float x_body = 0;
	float x_main = 0;
	float y = 0;

	float max_x;
};

std::deque<notify_t> notifications_standart;
std::deque<notify_t> notifications_big;

namespace notify
{
	void add(std::string pre, std::string body, Color color_pre, Color color_body, int life_time, log_type type)
	{
		switch (type)
		{
		case console:
			if (!g_engine_client || !g_cvar)
			{
				return;
			}
			g_cvar->console_dprintf("[ ");
			g_cvar->console_color_printf(color_pre, pre.data());
			g_cvar->console_dprintf(" ] ");
			g_cvar->console_color_printf(color_body, body.data());
			g_cvar->console_dprintf("\n");
			break;
		case screen_standart:
			notifications_standart.push_front(notify_t{ pre, body, color_pre, color_body, (float)life_time });
			break;
		case screen_big:
			notifications_big.push_front(notify_t{ "", body, color_pre, Color(0,0,0,0), (float)life_time });
			break;
		}
	}

	void add(std::string body, int life_time)
	{
		add("", body, settings::misc::menu_color, Color(255, 255, 255, 255), life_time, log_type::screen_big);
	}

	void add(std::string body)
	{
		add("", body, settings::misc::menu_color, Color(255, 255, 255, 255), 2000.f, log_type::screen_big);
	}

	void add(std::string pre, std::string body)
	{
		add(pre, body, settings::misc::menu_color, Color(255, 255, 255, 255), 2000.f, log_type::screen_standart);
	}

	void add(std::string pre, std::string body, int life_time)
	{
		add(pre, body, settings::misc::menu_color, Color(255, 255, 255, 255), life_time, log_type::screen_standart);
	}

	void add(std::string pre, std::string body, Color color_pre, Color color_body)
	{
		add(pre, body, color_pre, color_body, 2000.f, log_type::screen_standart);
	}

	void add(std::string pre, std::string body, Color color_pre, int life_time, log_type type)
	{
		add(pre, body, color_pre, Color(255, 255, 255, 255), life_time, type);
	}
	void add(std::string pre, std::string body, Color color_pre, log_type type)
	{
		add(pre, body, color_pre, Color(255, 255, 255, 255), 2000.f, type);
	}
	void add(std::string pre, std::string body, log_type type)
	{
		add(pre, body, settings::misc::menu_color, Color(255, 255, 255, 255), 2000.f, type);
	}

	void render()
	{
		int screen_w, screen_h;

		g_engine_client->get_screen_size(screen_w, screen_h);

		if (!notifications_standart.empty())
		{
			for (size_t i = 0; i < notifications_standart.size(); i++)
			{
				auto& notify = notifications_standart.at(i);

				int left_size = small_font->CalcTextSizeA(12.f, FLT_MAX, 0.f, "[ ").x;
				int right_size = small_font->CalcTextSizeA(12.f, FLT_MAX, 0.f, " ]").x;
				int space_size = small_font->CalcTextSizeA(12.f, FLT_MAX, 0.f, "  ").x;

				std::string all_text = utils::snprintf("[ %s ] %s", notify.s_pre.c_str(), notify.s_body.c_str());

				ImVec2 pre_size = small_font->CalcTextSizeA(12.f, FLT_MAX, 0.0f, notify.s_pre.c_str());
				ImVec2 all_text_size = small_font->CalcTextSizeA(12.f, FLT_MAX, 0.f, all_text.c_str());
				notify.max_x = all_text_size.x + 10;

				notify.y = utils::lerp(notify.y, i * (all_text_size.y + 3), 0.03f);

				if (notify.y > screen_h + 10 || notify.x_main < -3)
				{
					notifications_standart.erase(notifications_standart.begin() + i);
					continue;
				}

				if (notify.time_life == 0 || notify.time_life >= utils::epoch_time())
				{
					notify.x_main = utils::lerp(notify.x_main, notify.max_x + 5, 0.06f);

					if (notify.x_main > notify.max_x - (notify.max_x / 3))
					{
						notify.x_body = utils::lerp(notify.x_body, notify.max_x + 3, 0.05f);

						if (notify.x_body >= notify.max_x && notify.time_life == 0)
						{
							notify.time_life = utils::epoch_time() + notify.time_del;
						}
					}
				}
				else
				{
					notify.x_body = utils::lerp(notify.x_body, -20, 0.06f);

					if (notify.x_body < notify.max_x / 3)
					{
						notify.x_main = utils::lerp(notify.x_main, -20, 0.07f);
					}
				}

				int procent_x = std::clamp((100 * notify.x_body) / notify.max_x, 0.f, 100.f);
				int opacity = (255.f / 100.f) * procent_x;

				notify.c_body = Color(notify.c_body, opacity);
				notify.c_pre = Color(notify.c_pre, opacity);

				int start_text = notify.x_body - all_text_size.x - 7;

				render::draw_box_filled(0.f, notify.y, notify.x_body, notify.y + all_text_size.y + 3, Color(0, 0, 0, 100));
				render::draw_box_filled(notify.x_body, notify.y, notify.x_main, notify.y + all_text_size.y + 3, Color(notify.c_pre, 255));

				render::draw_text("[", ImVec2(start_text, notify.y + 2), 12.f, Color(255, 255, 255, 255), false, false);
				render::draw_text(" ]", ImVec2(start_text + pre_size.x + left_size + 1, notify.y + 1), 12.f, Color(255, 255, 255, 255), false, false);
				render::draw_text(notify.s_pre, ImVec2(start_text + left_size + 1, notify.y + 2), 12.f, notify.c_pre, false, false);
				render::draw_text(notify.s_body, ImVec2(start_text + left_size + right_size + space_size + pre_size.x + 2, notify.y + 1), 12.f, notify.c_body, false, false);
			}
		}

		if (!notifications_big.empty())
		{
			for (size_t i = 0; i < notifications_big.size(); i++)
			{
				auto& notify = notifications_big.at(i);

				float round = ImGui::GetStyle().FrameRounding;

				ImVec2 text_size = small_font->CalcTextSizeA(12.f, FLT_MAX, 0.0f, notify.s_body.c_str());

				float box_size_x = 50.f + text_size.x;
				float box_size_y = 40.f;

				float procent_x = 100;//(100 * (notify.time_del - utils::epoch_time())) / notify.time_life;

				if (notify.x_body == 0 && notify.y == 0)
				{
					notify.x_body = screen_w + box_size_x;
					notify.y = screen_h + box_size_y;
				}
	
				float start_pos_x = screen_w - 100.f;
				float start_pos_y = screen_h - (i * box_size_y) - (i * 15.f) - 50.f;

				if (notify.time_life == 0 || notify.time_life >= utils::epoch_time())
				{
					notify.x_body = utils::lerp(notify.x_body, start_pos_x, 0.03f);
					notify.y = utils::lerp(notify.y, start_pos_y, 0.03f);

					if (notify.time_life == 0 && notify.x_body < start_pos_x + 2 && notify.y < start_pos_y + 2)
						notify.time_life = utils::epoch_time() + notify.time_del;
				}
				else
				{
					notify.x_body = utils::lerp(notify.x_body, screen_w + box_size_x + 40, 0.03f);
				}


				if (notify.x_body > screen_w + box_size_x + 4 || notify.y < 0)
				{
					notifications_big.erase(notifications_big.begin() + i);
					continue;
				}

				if (notify.time_life != 0)
				{
					procent_x = (100 * (notify.time_life - utils::epoch_time())) / notify.time_del;
				}

				float opacity = std::clamp((box_size_y / 100.f) * procent_x, 0.f, box_size_y);

				render::draw_box_filled_round(notify.x_body - box_size_x,       notify.y - box_size_y, notify.x_body, notify.y, Color(50, 50, 50, 200), 0.f, round);
				render::draw_box_filled_round(notify.x_body - box_size_x - 4.f, notify.y - box_size_y, notify.x_body - box_size_x, notify.y, Color(notify.c_pre, 255), 1.f, round);
				render::draw_box_filled      (notify.x_body - box_size_x,       notify.y - opacity, notify.x_body - box_size_x + 2.f, notify.y, Color(notify.c_pre, 255));

				render::draw_text(notify.s_body, notify.x_body - (box_size_x / 2), notify.y - (box_size_y / 2) - (text_size.y / 2), 12, Color(255, 255, 255, 255), true, false);
			}
		}

	}

	void events(i_game_event* event)
	{
		if (!settings::visuals::events::enable)
			return;

		static auto get_hitgroup_name = [](int hitgroup) -> const char* {
			switch (hitgroup)
			{
			case hit_head:
				return "head";
			case hit_left_foot:
				return "left leg";
			case hit_right_foot:
				return "right leg";
			case hit_chest:
				return "chest";
			case hit_lower_chest:
				return "lower chest";
			case hit_left_hand:
				return "left arm";
			case hit_right_hand:
				return "right arm";
			case hit_neck:
				return "neck";
			default:
				return "body";
			}
		};

		constexpr char* hasdefusekit[2] = { "without defuse kit.","with defuse kit." };
		constexpr char* hasbomb[2] = { "without the bomb.","with the bomb." };

		if (strstr(event->get_name(), "player_hurt"))
		{
			auto userid = event->get_int("userid");
			auto attacker = event->get_int("attacker");

			if (!userid || !attacker)
				return;

			auto userid_id = g_engine_client->get_player_for_user_id(userid);
			auto attacker_id = g_engine_client->get_player_for_user_id(attacker);

			player_info_t userid_info, attacker_info;
			if (!g_engine_client->get_player_info(userid_id, &userid_info))
				return;

			if (!g_engine_client->get_player_info(attacker_id, &attacker_info))
				return;

			auto m_victim = static_cast<c_base_player*>(g_entity_list->get_client_entity(userid_id));

			Color color;
			std::string notify_text;
			bool can_show = false;

			if (attacker_id == g_engine_client->get_local_player() && userid_id != g_engine_client->get_local_player())
			{
				notify_text = utils::snprintf("you hurt %s in the %s for %i (%i health remaining)", userid_info.szName, get_hitgroup_name(event->get_int("hitgroup")), event->get_int("dmg_health"), event->get_int("health"));
				color = settings::visuals::events::hurt.custom_color ? settings::visuals::events::hurt.color : settings::misc::menu_color;

				can_show = true;
			}
			else if (userid_id == g_engine_client->get_local_player() && attacker_id != g_engine_client->get_local_player())
			{
				notify_text = utils::snprintf("you took %i damage from %s in the %s (%i health remaining)", event->get_int("dmg_health"), attacker_info.szName, get_hitgroup_name(event->get_int("hitgroup")), event->get_int("health"));
				color = settings::visuals::events::hurt.custom_color ? settings::visuals::events::hurt.color : settings::misc::menu_color;

				can_show = true;
			}

			if (can_show)
			{
				if (settings::visuals::events::hurt.enable_screen)
					add("damage", notify_text, color);
				if (settings::visuals::events::hurt.enable_console)
					add("damage", notify_text, color, log_type::console);
			}
		}

		if (strstr(event->get_name(), "item_purchase"))
		{
			auto userid = event->get_int("userid");

			if (!userid)
				return;

			auto userid_id = g_engine_client->get_player_for_user_id(userid);

			player_info_t userid_info;
			if (!g_engine_client->get_player_info(userid_id, &userid_info))
				return;

			auto m_player = static_cast<c_base_player*>(g_entity_list->get_client_entity(userid_id));

			if (!g_local_player || !m_player)
				return;

			if (m_player->team_num() == g_local_player->team_num())
				return;

			std::string notify_text = utils::snprintf("%s purchased a(n) %s", userid_info.szName, event_to_normal[event->get_string("weapon")].c_str());
			Color color = settings::visuals::events::player_buy.custom_color ? settings::visuals::events::player_buy.color : settings::misc::menu_color;

			if (settings::visuals::events::player_buy.enable_screen)
				add("buy", notify_text, color);
			if (settings::visuals::events::player_buy.enable_console)
				add("buy", notify_text, color, log_type::console);
		}

		if (strstr(event->get_name(), "bomb_beginplant"))
		{
			auto userid = event->get_int("userid");

			if (!userid)
				return;

			auto userid_id = g_engine_client->get_player_for_user_id(userid);

			player_info_t userid_info;
			if (!g_engine_client->get_player_info(userid_id, &userid_info))
				return;

			auto m_player = static_cast<c_base_player*>(g_entity_list->get_client_entity(userid_id));

			if (!m_player || m_player == g_local_player)
				return;

			std::string namemap = g_engine_client->get_level_name_short();
			std::string bomb_site_s = bomb_site(event->get_int("site"), namemap);

			std::string notify_text;
			Color color = settings::visuals::events::planting.custom_color ? settings::visuals::events::planting.color : settings::misc::menu_color;

			if (bomb_site_s != "unck")
			{
				notify_text = utils::snprintf("%s has begin planting the bomb at site %s", userid_info.szName, bomb_site_s.c_str());
			}
			else if (bomb_site_s == "unck")
			{
				notify_text = utils::snprintf("%s has begin planting the bomb", userid_info.szName, bomb_site_s.c_str());
			}

			if (settings::visuals::events::planting.enable_screen)
				add("info", notify_text, color);
			if (settings::visuals::events::planting.enable_console)
				add("info", notify_text, color, log_type::console);
		}

		if (strstr(event->get_name(), "bomb_planted"))
		{
			auto userid = event->get_int("userid");

			if (!userid)
				return;

			auto userid_id = g_engine_client->get_player_for_user_id(userid);

			player_info_t userid_info;
			if (!g_engine_client->get_player_info(userid_id, &userid_info))
				return;

			auto m_player = static_cast<c_base_player*>(g_entity_list->get_client_entity(userid_id));

			if (!m_player || m_player == g_local_player)
				return;

			std::string namemap = g_engine_client->get_level_name_short();
			std::string bomb_site_s = bomb_site(event->get_int("site"), namemap);

			std::string notify_text;
			Color color = settings::visuals::events::planting.custom_color ? settings::visuals::events::planting.color : settings::misc::menu_color;

			if (bomb_site_s != "unck")
			{
				notify_text = utils::snprintf("%s has planted bomb at site %s", userid_info.szName, bomb_site_s.c_str());
			}
			else if (bomb_site_s == "unck")
			{
				notify_text = utils::snprintf("%s has planted the bomb", userid_info.szName, bomb_site_s.c_str());
			}

			if (settings::visuals::events::planting.enable_screen)
				add("info", notify_text, color);
			if (settings::visuals::events::planting.enable_console)
				add("info", notify_text, color, log_type::console);
		}

		if (strstr(event->get_name(), "bomb_begindefuse"))
		{
			auto userid = event->get_int("userid");

			if (!userid)
				return;

			auto userid_id = g_engine_client->get_player_for_user_id(userid);

			player_info_t userid_info;
			if (!g_engine_client->get_player_info(userid_id, &userid_info))
				return;

			auto m_player = static_cast<c_base_player*>(g_entity_list->get_client_entity(userid_id));

			if (!m_player || m_player == g_local_player)
				return;

			std::string notify_text = utils::snprintf("%s has began defusing the bomb %s", userid_info.szName, hasdefusekit[event->get_bool("haskit")]);
			Color color = settings::visuals::events::defusing.custom_color ? settings::visuals::events::defusing.color : settings::misc::menu_color;

			globals::misc::bomb_defusing_with_kits = event->get_bool("haskit");

			if (settings::visuals::events::defusing.enable_screen)
				add("info", notify_text, color);
			if (settings::visuals::events::defusing.enable_console)
				add("info", notify_text, color, log_type::console);
		}
	}
}