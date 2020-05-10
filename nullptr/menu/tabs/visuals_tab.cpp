#include "../menu.h"
#include "../../helpers/sound.h"

int chams_tab_player_e_layer = 0;
int chams_tab_player_t_layer = 0;
int chams_tab_player_l_layer = 0;
int chams_tab_ragdoll_e_layer = 0;
int chams_tab_ragdoll_t_layer = 0;
int chams_tab_backtrack_layer = 0;
int chams_tab_hands_layer = 0;
int chams_tab_weapon_layer = 0;
int chams_tab_weapon_dopped_layer = 0;

int chams_selected = 0;
int glow_player_tab = 0;
int chams_player_tab = 0;
int chams_player_tab_ragdoll = 0;
int chams_weapon_tab = 0;
int esp_player_tab = 0;

int& get_tab_chams(int tab, int main)
{
	switch (tab)
	{
	case 0:
		if (main == 0) return chams_tab_player_e_layer;
		else if (main == 1) return chams_tab_player_t_layer;
		else if (main == 2) return chams_tab_player_l_layer;
		break;
	case 1:
		if (main == 0) return chams_tab_ragdoll_e_layer;
		else if (main == 1) return chams_tab_ragdoll_t_layer;
		break;
	}
}

namespace menu
{
	void tabs::visuals_tab()
	{
		if (general_tab == 0)
		{
			auto settings_cur = &settings::visuals::esp::esp_items[esp_player_tab];

			ImGui::Columns(2, nullptr, false);
			ImGui::BeginChild("general##child", ImVec2(0, 77), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				horizontal_tabs(esp_player_tab, players_tabs);

				ImGui::Checkbox("enable", &settings_cur->enable);
			}
			ImGui::EndChild("general##child");
			ImGui::BeginChild("player", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				ImGui::Checkbox("visible only", &settings_cur->only_visible);
				ImGui::Checkbox("skeleton", &settings_cur->skeleton);
				ImGui::Checkbox("box", &settings_cur->box);
				ImGui::Checkbox("name", &settings_cur->name);
				ImGui::Checkbox("health bar [?]", &settings_cur->health_bar);
				if (ImGui::ItemsToolTipBegin("##health_bartooltip"))
				{
					ImGui::Checkbox("health in bar", &settings_cur->health_in_bar);
					ImGui::Checkbox("color healthbased", &settings_cur->health_based);
					ImGui::Combo("health bar position", &settings_cur->health_position, std::vector<std::string>{ "left", "right", "top", "bottom" });

					ImGui::ItemsToolTipEnd();
				}

				ImGui::Checkbox("armour bar [?]", &settings_cur->armor_bar);

				if (ImGui::ItemsToolTipBegin("##armour_bartooltip"))
				{
					ImGui::Checkbox("armour in bar", &settings_cur->armor_in_bar);
					ImGui::Combo("armor bar position", &settings_cur->armor_position, std::vector<std::string>{ "left", "right", "top", "bottom" });

					ImGui::ItemsToolTipEnd();
				}

				ImGui::Checkbox("weapon [?]", &settings_cur->weapon);
				if (ImGui::ItemsToolTipBegin("##weapon_bartooltip"))
				{
					ImGui::Checkbox("ammo in bar", &settings_cur->ammo_in_bar);
					ImGui::Checkbox("ammo bar", &settings_cur->weapon_ammo);

					ImGui::ItemsToolTipEnd();
				}

				const char* flags_c[] = { "scoped", "flashed", "defusing", "planting", "reloading", "bomb", "armor", "helmet" };
				bool* flags_b[] = { &settings_cur->flags_scoped, &settings_cur->flags_flashed, &settings_cur->flags_defusing, &settings_cur->flags_planting, &settings_cur->flags_reloading, &settings_cur->flags_bomb_carrier, &settings_cur->flags_armor, &settings_cur->flags_helmet };
				ImGui::MultiCombo("flags", flags_c, flags_b, 8);

			}
			ImGui::EndChild("player");
			ImGui::NextColumn();
			ImGui::BeginChild("colors##players_visial", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
			{

				ImGui::ColorEdit3("dormant", &settings_cur->dormant);
				ImGui::NewLine();
				ImGui::ColorEdit3("visible", &settings_cur->box_visible);
				ImGui::ColorEdit3("invisible", &settings_cur->box_invisible);

				ImGui::NewLine();

				ImGui::ColorEdit3("skeleton visible", &settings_cur->skeleton_visible);
				ImGui::ColorEdit3("skeleton invisible", &settings_cur->skeleton_invisible);

				ImGui::NewLine();

				ImGui::ColorEdit3("health bar outline", &settings_cur->health_bar_outline);
				ImGui::ColorEdit3("health bar background", &settings_cur->health_bar_background);
				ImGui::ColorEdit3("health bar main", &settings_cur->health_bar_main);

				ImGui::NewLine();

				ImGui::ColorEdit3("armor bar outline", &settings_cur->armor_bar_outline);
				ImGui::ColorEdit3("armor bar background", &settings_cur->armor_bar_background);
				ImGui::ColorEdit3("armor bar main", &settings_cur->armor_bar_main);

				ImGui::NewLine();

				ImGui::ColorEdit3("ammo bar outline", &settings_cur->ammo_bar_outline);
				ImGui::ColorEdit3("ammo bar background", &settings_cur->ammo_bar_background);
				ImGui::ColorEdit3("ammo bar main", &settings_cur->ammo_bar_main);

			}
			ImGui::EndChild("colors##players_visial");
		}
		if (general_tab == 1)
		{
			auto settings_cur = &settings::visuals::glow::glow_items[glow_player_tab];

			static char* glow_types[] = { "standart outline", "glow pulsing", "inner outline", "inner outline pulsing" };

			ImGui::Columns(2, nullptr, false);
			ImGui::BeginChild("general##childglow", ImVec2(0, 77), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				horizontal_tabs(glow_player_tab, players_tabs);

				ImGui::Checkbox("enable", &settings_cur->enable);
			}
			ImGui::EndChild("general##childglow");
			ImGui::BeginChild("players##visuals_players", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				if (glow_player_tab != 2)
				{
					ImGui::Checkbox("only visible", &settings_cur->visible_only);
					ImGui::ColorEdit3("visible color", &settings_cur->visible);
					ImGui::ColorEdit3("invisible color", &settings_cur->in_visible);
				}
				else
				{
					ImGui::ColorEdit3("color", &settings_cur->visible);
				}
			}
			ImGui::EndChild("players##visuals_players");
		}
		if (general_tab == 2)
		{
			horizontal_tabs(chams_selected, tabs_chams);

			if (chams_selected == 0)
			{
				chams_settings* settings_cur = nullptr;

				ImGui::Columns(2, nullptr, false);
				ImGui::BeginChild("general##childchams", ImVec2(0, 101), true, ImGuiWindowFlags_ChildWindowTitle);
				{
					horizontal_tabs(chams_player_tab, players_tabs);					
					horizontal_tabs(get_tab_chams(0, chams_player_tab), layer_tabs);
					settings_cur = get_tab_chams(0, chams_player_tab) == 0 ? &settings::visuals::chams::player_settings[chams_player_tab].bot : &settings::visuals::chams::player_settings[chams_player_tab].top;

					ImGui::Checkbox("enable##chams", &settings_cur->enable);
				}
				ImGui::EndChild("general##childchams");
				ImGui::BeginChild("players##visuals_chams", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
				{

					ImGui::Checkbox("visible only", &settings_cur->only_visible);

					ImGui::Combo("type", &settings_cur->chams_type, chams_types, IM_ARRAYSIZE(chams_types));

					ImGui::ColorEdit3("color visible", &settings_cur->visible);
					ImGui::ColorEdit3("color invisible", &settings_cur->invisible);

					ImGui::NewLine();
				}
				ImGui::EndChild("players##visuals_chams");
				ImGui::NextColumn();

				ImGui::BeginChild("backtrack##visuals_chams_backtrack", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
				{
					horizontal_tabs(chams_tab_backtrack_layer, layer_tabs);
					chams_settings* settings_cur_backtrak = chams_tab_backtrack_layer == 0 ? &settings::visuals::chams::backtrack_settings.bot : &settings::visuals::chams::backtrack_settings.top;
					ImGui::Checkbox("enable##visuals_chams_backtrack", &settings_cur_backtrak->enable);
					ImGui::Combo("draw##visuals_chams_backtrack", &settings::visuals::chams::tick_draw, std::vector<std::string>{ "last tick", "all ticks" });
					ImGui::Checkbox("visible only##visuals_chams_backtrack", &settings_cur_backtrak->only_visible);
					ImGui::Combo("type##visuals_chams_backtrack", &settings_cur_backtrak->chams_type, chams_types, IM_ARRAYSIZE(chams_types));
					ImGui::ColorEdit3("color visible##color_handsv3123123isuals_chams_backtrack", &settings_cur_backtrak->visible);
					ImGui::ColorEdit3("color invisible##color_handsvisuals_chams_backtrack", &settings_cur_backtrak->invisible);
				}
				ImGui::EndChild("backtrack##visuals_chams_backtrack");
			}
			else if (chams_selected == 1)
			{
				chams_settings* settings_cur = nullptr;

				ImGui::Columns(2, nullptr, false);
				ImGui::BeginChild("general##childchams", ImVec2(0, 101), true, ImGuiWindowFlags_ChildWindowTitle);
				{
					horizontal_tabs(chams_player_tab_ragdoll, players_tabs_ragdoll);
					horizontal_tabs(get_tab_chams(1, chams_player_tab_ragdoll), layer_tabs);
					settings_cur = get_tab_chams(1, chams_player_tab_ragdoll) == 0 ? &settings::visuals::chams::ragdoll_settings[chams_player_tab_ragdoll].bot : &settings::visuals::chams::ragdoll_settings[chams_player_tab_ragdoll].top;

					ImGui::Checkbox("enable##chams", &settings_cur->enable);
				}
				ImGui::EndChild("general##childchams");

				ImGui::BeginChild("ragdolls##visuals_chams", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
				{

					ImGui::Checkbox("visible only", &settings_cur->only_visible);

					ImGui::Combo("type", &settings_cur->chams_type, chams_types, IM_ARRAYSIZE(chams_types));

					ImGui::ColorEdit3("color visible", &settings_cur->visible);
					ImGui::ColorEdit3("color invisible", &settings_cur->invisible);

					ImGui::NewLine();
				}
				ImGui::EndChild("ragdolls##visuals_chams");
				ImGui::NextColumn();

				ImGui::BeginChild("hand's##visuals_chams_hands", ImVec2(0, 112), true, ImGuiWindowFlags_ChildWindowTitle);
				{
					horizontal_tabs(chams_tab_hands_layer, layer_tabs);

					chams_settings* cur_settings = chams_tab_hands_layer == 0 ? &settings::visuals::chams::hands_settings.bot : &settings::visuals::chams::hands_settings.top;

					ImGui::Checkbox("enable##visuals_chams_hands", &cur_settings->enable);
					ImGui::SameLine();
					ImGui::ColorEdit3("##color_handsvisuals_chams", &cur_settings->visible);
					ImGui::Combo("type##visuals_chams_hands", &cur_settings->chams_type, chams_types, IM_ARRAYSIZE(chams_types));
				}
				ImGui::EndChild("hand's##visuals_chams");

				ImGui::BeginChild("weapon##visuals_chams_weapon", ImVec2(0, 136), true, ImGuiWindowFlags_ChildWindowTitle);
				{
					chams_settings* cur_settings = nullptr;

					char* tabs_local[] = {"in hand", "dropped"};

					horizontal_tabs(chams_weapon_tab, tabs_local);
					horizontal_tabs(chams_weapon_tab == 0 ? chams_tab_weapon_layer : chams_tab_weapon_dopped_layer, layer_tabs);
					if (chams_weapon_tab == 1)
					{
						cur_settings = chams_tab_weapon_dopped_layer == 0 ? &settings::visuals::chams::weapon_dropped_settings.bot : &settings::visuals::chams::weapon_dropped_settings.top;
					}
					else
					{
						cur_settings = chams_tab_weapon_layer == 0 ? &settings::visuals::chams::weapon_settings.bot : &settings::visuals::chams::weapon_settings.top;
					}

					ImGui::Checkbox("enable##visuals_chams_weapon", &cur_settings->enable);
					ImGui::SameLine();
					ImGui::ColorEdit3("##color_handsvisuals_chams_weapon", &cur_settings->visible);
					ImGui::Combo("type##visuals_chams_weapon", &cur_settings->chams_type, chams_types, IM_ARRAYSIZE(chams_types));
				}
				ImGui::EndChild("weapon##visuals_chams_weapon");
			}
		}
		if (general_tab == 3)
		{
			ImGui::Columns(2, nullptr, false);

			ImGui::BeginChild("general##other_tab", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				ImGui::Checkbox("offscreen [?]", &settings::visuals::ofc::enable);
				if (ImGui::ItemsToolTipBegin("##offscreenother_tab"))
				{
					ImGui::Checkbox("visible check", &settings::visuals::ofc::visible_check);

					ImGui::SliderFloat("range##offscreen", &settings::visuals::ofc::range, 0.f, 100.f, "%.1f");
					ImGui::SliderFloat("wight##offscreen", &settings::visuals::ofc::wight, 0.f, 30.f, "%.1f");
					ImGui::SliderFloat("hight##offscreen", &settings::visuals::ofc::hight, 0.f, 30.f, "%.1f");

					ImGui::ItemsToolTipEnd();
				}
				ImGui::SameLine();
				ImGui::ColorEdit4("##color_offscreen", &settings::visuals::ofc::color);

				ImGui::Checkbox("dropped weapons [?]", &settings::visuals::dropped_weapon::enable);
				if (ImGui::ItemsToolTipBegin("##dropped_weapons_other_tab"))
				{
					ImGui::Checkbox("box##dropped_weapons", &settings::visuals::dropped_weapon::box);
					ImGui::SameLine();
					ImGui::ColorEdit4("##colorbox_dropped_weapons", &settings::visuals::dropped_weapon::box_color);

					ImGui::Checkbox("ammo bar##dropped_weapons", &settings::visuals::dropped_weapon::ammo_bar);
					ImGui::Checkbox("ammo in bar##dropped_weapons", &settings::visuals::dropped_weapon::ammo_in_bar);

					ImGui::ColorEdit3("outline##dropped_weapons", &settings::visuals::dropped_weapon::bar_outline);
					ImGui::ColorEdit3("background##dropped_weapons", &settings::visuals::dropped_weapon::bar_background);
					ImGui::ColorEdit3("main##dropped_weapons", &settings::visuals::dropped_weapon::bar_main);


					ImGui::ItemsToolTipEnd();
				}
				ImGui::Checkbox("damage indicator [?]", &settings::visuals::damage_indicator::enable);
				if (ImGui::ItemsToolTipBegin("##damage_indicator_tooltip"))
				{
					ImGui::SliderFloat("show time##damage_indicator_tooltip", &settings::visuals::damage_indicator::show_time, 2.f, 10.f, "%.1f");
					ImGui::SliderFloat("speed##damage_indicator_tooltip", &settings::visuals::damage_indicator::speed, 0.f, 1.f, "%.2f");

					ImGui::SliderFloat("max position y##damage_indicator_tooltip", &settings::visuals::damage_indicator::max_pos_y, 0.f, 200.f, "%.1f");

					ImGui::SliderFloat("offset at kill##damage_indicator_tooltip", &settings::visuals::damage_indicator::offset_if_kill, 0.f, 20.f, "%.1f");
					ImGui::SliderFloat("offset at hit##damage_indicator_tooltip", &settings::visuals::damage_indicator::offset_hit, 0.f, 10.f, "%.1f");

					ImGui::SliderFloat("range offset x##damage_indicator_tooltip", &settings::visuals::damage_indicator::range_offset_x, 0.f, 30.f, "%.1f");

					ImGui::SliderFloat("text size##damage_indicator_tooltip", &settings::visuals::damage_indicator::text_size, 2.f, 20.f, "%.1f");
					ImGui::ColorEdit3("hit color##damage_indicator_tooltip", &settings::visuals::damage_indicator::standart_color);
					ImGui::ColorEdit3("kill color##damage_indicator_tooltip", &settings::visuals::damage_indicator::kill_color);

					ImGui::ItemsToolTipEnd();
				}
				ImGui::Checkbox("client bullet impact [?]", &settings::visuals::bullet_impact::box::enable_c);
				if (ImGui::ItemsToolTipBegin("##client_bullet_impact_rooltip"))
				{
					ImGui::SliderFloat("show time##client_bullet_imapct_show", &settings::visuals::bullet_impact::box::time_c, 0.f, 10.f, "%.1f");
					ImGui::SliderFloat("size##client_bullet_impact_size", &settings::visuals::bullet_impact::box::size_c, 0.f, 4.f, "%.1f");

					ImGui::ItemsToolTipEnd();
				}
				ImGui::SameLine();
				ImGui::ColorEdit3("##client_bullet_imapcct_color", &settings::visuals::bullet_impact::box::color_c);
				ImGui::Checkbox("server bullet impact [?]", &settings::visuals::bullet_impact::box::enable_s);
				if (ImGui::ItemsToolTipBegin("##server_bullet_impact_rooltip"))
				{
					ImGui::SliderFloat("show time##server_bullet_imapct_show", &settings::visuals::bullet_impact::box::time_s, 0.f, 10.f, "%.1f");
					ImGui::SliderFloat("size##server_bullet_impact_size", &settings::visuals::bullet_impact::box::size_s, 0.f, 4.f, "%.1f");

					ImGui::ItemsToolTipEnd();
				}
				ImGui::SameLine();
				ImGui::ColorEdit3("##server_bullet_imapcct_color", &settings::visuals::bullet_impact::box::color_s);
				ImGui::Checkbox("bullet tracers", &settings::visuals::bullet_impact::line::enable);

				const char* players_c[] = { "local player", "teammates", "enemies" };
				bool* players_b[]{ &settings::visuals::bullet_impact::line::local_player, &settings::visuals::bullet_impact::line::teammates, &settings::visuals::bullet_impact::line::enemies };
				ImGui::MultiCombo("players [?]##bullet_tracers", players_c, players_b, 3);
				if (ImGui::ItemsToolTipBegin("##bullet_tracers_tooltip"))
				{
					ImGui::ColorEdit3("color local##bullet_traces_color", &settings::visuals::bullet_impact::line::color_local);
					ImGui::ColorEdit3("color teammate##bullet_traces_color", &settings::visuals::bullet_impact::line::color_teammates);
					ImGui::ColorEdit3("color enemy##bullet_traces_color", &settings::visuals::bullet_impact::line::color_enemies);

					ImGui::ItemsToolTipEnd();
				}
				ImGui::SliderFloat("show time##bullet_tracer", &settings::visuals::bullet_impact::line::time, 0.f, 10.f, "%.1f");
				ImGui::Checkbox("hitmarker", &settings::misc::hitmarker);
				ImGui::Checkbox("hitsound", &settings::misc::hitsound::enable);
				ImGui::SliderFloat("hitsound volume", &settings::misc::hitsound::volume, 0.f, 1.f);
			}
			ImGui::EndChild("general##other_tab");

			ImGui::NextColumn();

			ImGui::BeginChild("other##other_tab", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				ImGui::Checkbox("capsule hitbox", &settings::visuals::hitbox::enable);
				ImGui::SameLine();
				ImGui::ColorEdit3("##hitbox_color", &settings::visuals::hitbox::color);
				ImGui::Combo("draw type##capsule_hitbox", &settings::visuals::hitbox::show_type, std::vector<std::string>{ "on hit", "on death" });
				ImGui::SliderFloat("show time##capsule_hitbox", &settings::visuals::hitbox::show_time, 0.f, 10.f, "%.1f");
			}
			ImGui::EndChild("other##other_tab");
		}
		if (general_tab == 4)
		{
			ImGui::Columns(2, nullptr, false);
			ImGui::BeginChild("general##visuals_general1", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				ImGui::Checkbox("grenade prediction [?]", &settings::visuals::grenade_prediction::enable);
				if (ImGui::ItemsToolTipBegin("grenade prediction"))
				{
					ImGui::Checkbox("radius##genpred", &settings::visuals::grenade_prediction::radius);
					ImGui::SameLine();
					ImGui::ColorEdit3("##radiuscolor_genpred", &settings::visuals::grenade_prediction::radius_color);

					ImGui::SliderFloat("line thickness##genpred", &settings::visuals::grenade_prediction::line_thickness, 0.f, 5.f, "%.1f");
					ImGui::SliderFloat("colision box size##genpred", &settings::visuals::grenade_prediction::colision_box_size, 0.f, 10.f, "%.1f");
					ImGui::SliderFloat("main colision box size##genpred", &settings::visuals::grenade_prediction::main_colision_box_size, 0.f, 10.f, "%.1f");

					ImGui::ColorEdit3("line color##genpred", &settings::visuals::grenade_prediction::main);
					ImGui::ColorEdit3("colision box color##genpred", &settings::visuals::grenade_prediction::main_box);
					ImGui::ColorEdit3("end colision box color##genpred", &settings::visuals::grenade_prediction::end_box);

					ImGui::ItemsToolTipEnd();
				}

				ImGui::Checkbox("draw fov basic [?]", &settings::visuals::fov::fov_basic);
				if (ImGui::ItemsToolTipBegin("fovsettings##basic"))
				{
					ImGui::ColorEdit3("color basic##basic", &settings::visuals::fov::color_fov_basic);
					ImGui::Checkbox("backgorund##basic", &settings::visuals::fov::fov_basic_background);
					if (settings::visuals::fov::fov_basic_background)
					{
						ImGui::SameLine();
						ImGui::ColorEdit3("##background_basic", &settings::visuals::fov::color_fov_basic_background);
					}
					ImGui::ItemsToolTipEnd();
				}

				ImGui::Checkbox("draw fov silent [?]", &settings::visuals::fov::fov_silent);
				if (ImGui::ItemsToolTipBegin("fovsettings##silent"))
				{
					ImGui::ColorEdit3("color silent##silent", &settings::visuals::fov::color_fov_silent);
					ImGui::Checkbox("backgorund##silent", &settings::visuals::fov::fov_silent_background);
					if (settings::visuals::fov::fov_silent_background)
					{
						ImGui::SameLine();
						ImGui::ColorEdit3("##fov_silent_backgound", &settings::visuals::fov::color_fov_silent_background);
					}
					ImGui::ItemsToolTipEnd();
				}

				ImGui::Checkbox("choke indicator", &settings::misc::choke_indicator);
				ImGui::Checkbox("angle lines", &settings::misc::desync::indicator::lines);
				ImGui::Checkbox("anti-aim arrow [?]", &settings::misc::desync::indicator::arrow);
				if (ImGui::ItemsToolTipBegin("##antiaim_arrow"))
				{
					ImGui::ColorEdit3("color real##antiaim_arrow", &settings::misc::desync::indicator::real);
					ImGui::ColorEdit3("color fake##antiaim_arrow", &settings::misc::desync::indicator::fake);
					ImGui::SliderFloat("size x##antiaim_arrow", &settings::misc::desync::indicator::size_x, 0.f, 30.f, "%.1f");
					ImGui::SliderFloat("size y##antiaim_arrow", &settings::misc::desync::indicator::size_y, 0.f, 30.f, "%.1f");
					ImGui::SliderFloat("offset x##antiaim_arrow", &settings::misc::desync::indicator::offset_x, 0.f, 200.f, "%.1f");
					ImGui::ItemsToolTipEnd();
				}
				ImGui::Checkbox("nightmode", &settings::misc::nightmode);
				ImGui::Checkbox("bomb timer", &settings::visuals::bomb_timer);
				ImGui::Checkbox("spreed circle", &settings::misc::spreed_circle::enable);
				ImGui::SameLine();
				ImGui::ColorEdit3("##spreed circle color", &settings::misc::spreed_circle::color);
				ImGui::Checkbox("sniper crosshair", &settings::misc::sniper_crosshair);
				const char* events_c[] = { "hit", "buy", "planting", "defusing", "configs", "lua" };

				bool* events_screen_b[6]{ &settings::visuals::events::hurt.enable_screen, &settings::visuals::events::player_buy.enable_screen, &settings::visuals::events::planting.enable_screen, &settings::visuals::events::defusing.enable_screen, &settings::visuals::events::config.enable_screen, &settings::visuals::events::lua.enable_screen };
				bool* events_console_b[6]{ &settings::visuals::events::hurt.enable_console, &settings::visuals::events::player_buy.enable_console, &settings::visuals::events::planting.enable_console, &settings::visuals::events::defusing.enable_console, &settings::visuals::events::config.enable_console, &settings::visuals::events::lua.enable_console };

				ImGui::Checkbox("events enable [?]", &settings::visuals::events::enable);
				if (ImGui::ItemsToolTipBegin("##customcolors"))
				{
					ImGui::Checkbox("hit log custom color", &settings::visuals::events::hurt.custom_color);
					ImGui::SameLine();
					ImGui::ColorEdit3("##hit_log_color", &settings::visuals::events::hurt.color);

					ImGui::Checkbox("buy log custom color", &settings::visuals::events::player_buy.custom_color);
					ImGui::SameLine();
					ImGui::ColorEdit3("##buy_log_color", &settings::visuals::events::player_buy.color);

					ImGui::Checkbox("planting log custom color", &settings::visuals::events::planting.custom_color);
					ImGui::SameLine();
					ImGui::ColorEdit3("##planting_log_color", &settings::visuals::events::planting.color);

					ImGui::Checkbox("defusing log custom color", &settings::visuals::events::defusing.custom_color);
					ImGui::SameLine();
					ImGui::ColorEdit3("##defusing_log_color", &settings::visuals::events::defusing.color);

					ImGui::Checkbox("configs log custom color", &settings::visuals::events::config.custom_color);
					ImGui::SameLine();
					ImGui::ColorEdit3("##configs_log_color", &settings::visuals::events::config.color);

					ImGui::Checkbox("lua log custom color", &settings::visuals::events::lua.custom_color);
					ImGui::SameLine();
					ImGui::ColorEdit3("##configs_log_color", &settings::visuals::events::lua.color);

					ImGui::ItemsToolTipEnd();
				}
				ImGui::MultiCombo("events on screen", events_c, events_screen_b, 6);
				ImGui::MultiCombo("events in console", events_c, events_console_b, 6);
			}
			ImGui::EndChild("general##visuals_general1");
			ImGui::NextColumn();
			ImGui::BeginChild("grenade's's##visuals_grenade's", ImVec2(0, 219), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				const char* time_type[] = { "bar", "text", "radius" };

				bool* smoke_time_types[3]{ &settings::visuals::grenades::smoke_bar, &settings::visuals::grenades::smoke_timer, &settings::visuals::grenades::smoke_radius};
				bool* molotov_time_types[3]{ &settings::visuals::grenades::molotov_bar, &settings::visuals::grenades::molotov_timer, &settings::visuals::grenades::molotov_radius };

				const char* grenadesarray[] = { "smoke", "flahsbang", "he grenade", "decoy", "molotov" };
				static int selected_grenade;

				ImGui::Checkbox("granade's##check", &settings::visuals::grenades::enable);

				ImGui::Combo("grenade", &selected_grenade, grenadesarray, IM_ARRAYSIZE(grenadesarray));

				if (selected_grenade == 0)
				{
					ImGui::ColorEdit3("name##smoke", &settings::visuals::grenades::color_smoke);
					ImGui::NewLine();
					ImGui::MultiCombo("time display type###smoke", time_type, smoke_time_types, 3);
					if (smoke_time_types[0])
					{
						ImGui::ColorEdit3("bar main##smoke", &settings::visuals::grenades::color_bar_smoke_main);
						ImGui::ColorEdit3("bar back##smoke", &settings::visuals::grenades::color_bar_smoke_back);
						ImGui::ColorEdit3("radius##smoke", &settings::visuals::grenades::color_smoke_radius);
					}
				}
				else if (selected_grenade == 1)
				{
					ImGui::ColorEdit3("name##flashbang", &settings::visuals::grenades::color_flash);
				}
				else if (selected_grenade == 2)
				{
					ImGui::ColorEdit3("name##hegrenade", &settings::visuals::grenades::color_frag);
				}
				else if (selected_grenade == 3)
				{
					ImGui::ColorEdit3("name##decoy", &settings::visuals::grenades::color_decoy);
				}
				else if (selected_grenade == 4)
				{
					ImGui::ColorEdit3("name##molotov", &settings::visuals::grenades::color_molotov);
					ImGui::NewLine();
					ImGui::MultiCombo("time display type##molotov", time_type, molotov_time_types, 3);
					if (molotov_time_types[0])
					{
						ImGui::ColorEdit3("bar main##molotov", &settings::visuals::grenades::color_bar_molotov_main);
						ImGui::ColorEdit3("bar back##molotov", &settings::visuals::grenades::color_bar_molotov_back);
						ImGui::ColorEdit3("radius##molotov", &settings::visuals::grenades::color_molotov_radius);
					}
				}
			}
			ImGui::EndChild("grenade's##visuals_grenade's");

			ImGui::BeginChild("fog##visualsfog", ImVec2(0, 109), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				ImGui::Checkbox("enable##fog", &settings::misc::fog::enable);
				ImGui::SameLine();
				ImGui::ColorEdit3("##color", &settings::misc::fog::color);

				float max_min = settings::misc::fog::end_dist - 10;

				max_min = std::clamp(max_min, 0.f, settings::misc::fog::end_dist);

				ImGui::SliderFloat("fog start dist", &settings::misc::fog::start_dist, 0.f, max_min, "%.0f");
				ImGui::SliderFloat("fog end dist", &settings::misc::fog::end_dist, 0.f, 10000.f, "%.0f");
			}
			ImGui::EndChild("fog##visualsfog");
		}
	}
}