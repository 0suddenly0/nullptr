#include "../menu.h"
#include "../../features/legit_aimbot/aimbot.h"

static int weapon_vector_index = 0;
struct WeaponName_t {
	constexpr WeaponName_t(int32_t definition_index, const char* name) :
		definition_index(definition_index),
		name(name) {
	}

	int32_t definition_index = 0;
	const char* name = nullptr;
};

std::vector< WeaponName_t> WeaponNames =
{
{228, "knife"},
{ 31, "zeus" },

{ 61, "usp-s" },
{ 4, "glock-18" },
{ 32, "p2000" },
{ 2,  "dual berettas" },
{ 36, "p250" },
{ 63, "cz75 auto" },
{ 3,  "five-seven" },
{ 30, "tec-9" },
{ 64, "r8 revolver" },
{ 1,  "desert eagle" },

{ 34, "mp9" },
{ 17, "mac-10" },
{ 33, "mp7" },
{ 23, "mp5-sd" },
{ 24, "ump-45" },
{ 19, "p90" },
{ 26, "pp-bizon" },

{ 13, "galil ar" },
{ 10, "famas" },
{ 7,  "ak-47" },
{ 60, "m4a1-s" },
{ 16, "m4a4" },
{ 8, "aug" },
{ 40, "ssg 08" },
{ 39, "sg 553" },
{ 9, "awp" },
{ 38, "scar-20" },
{ 11, "g3sg1" },

{ 35, "nova" },
{ 25, "xm1014" },
{ 29, "sawed-off" },
{ 27, "mag-7" },
{ 14, "m249" },
{ 28, "negev" },
};

void current_weapon_checkbox()
{
	ImGui::Checkbox("auto current", &settings::legit_bot::auto_current);

	if (!g_engine_client->is_connected() || !g_local_player || !g_local_player->is_alive())
	{
		return;
	}

	auto weapon = g_local_player->active_weapon();

	if (!weapon || !weapon->is_weapon())
	{
		return;
	}


	if (settings::legit_bot::auto_current)
	{
		int wpn_idx = legit_aimbot::get_knife_settings();// weapon->item().item_definition_index();
		auto wpn_it = std::find_if(WeaponNames.begin(), WeaponNames.end(), [wpn_idx](const WeaponName_t& a)
			{
				return a.definition_index == wpn_idx;
			});

		if (wpn_it != WeaponNames.end())
		{
			menu::settings_weap = wpn_idx;
			settings::legit_bot::settings_weapon = std::abs(std::distance(WeaponNames.begin(), wpn_it));
		}
	}
}

static char* weapon_groups_array[] =
{
	"knife",
"zeus",
"pistols",
"semi-rifle",
"shot gun",
"machin gun",
"rifle",
"sniper rifle"
};

static char* legit_tabs[] =
{
	"all",
	"groups",
	"on weapon"
};

static char* smooth_types[] = {
	"static",
	"dynamic"
};


const char* hitbox_list[7] = {
	"head",
	"neck",
	"pelvis",
	"stomach",
	"lower chest",
	"chest",
	"upper chest",
};

namespace menu
{
	void tabs::legit_tab()
	{
		legitbot_settings* settings;

		ImGui::Columns(2, NULL, false);
		ImGui::BeginChild("weapon select", ImVec2(0, 132), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			auto TabsW_players_tab = (ImGui::GetCurrentWindow()->Size.x - menu::_style.WindowPadding.x * 2.0f) / _countof(legit_tabs);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing_new, ImVec2(0, 0));
			{
				render_tabs(legit_tabs, settings::legit_bot::settings_category, TabsW_players_tab, 20.0f);
			}
			ImGui::PopStyleVar();

			if (settings::legit_bot::settings_category == 0)
			{
				ImGui::Dummy(ImVec2(0.f, 51.f));
				settings = &settings::legit_bot::legitbot_items_all;
			}
			else if (settings::legit_bot::settings_category == 1)
			{
				ImGui::Combo("group##weapons_groups", &settings::legit_bot::weapon_group, weapon_groups_array, IM_ARRAYSIZE(weapon_groups_array));
				settings = &settings::legit_bot::legitbot_items_groups[settings::legit_bot::weapon_group];

				ImGui::Dummy(ImVec2(0.f, 16.f));
			}
			else if (settings::legit_bot::settings_category == 2)
			{
				if (!settings::legit_bot::auto_current)
				{
					if (ImGui::Combo(
						"weapon", &settings::legit_bot::settings_weapon,
						[](void* data, int32_t idx, const char** out_text)
						{
							auto vec = reinterpret_cast<std::vector< WeaponName_t >*>(data);
							*out_text = vec->at(idx).name;
							return true;
						}, (void*)(&WeaponNames), WeaponNames.size()))
					{
						settings_weap = WeaponNames[settings::legit_bot::settings_weapon].definition_index;
					}
				}
				else
				{
					ImGui::TextS("settings weapon : %s", WeaponNames[settings::legit_bot::settings_weapon].name);
				}

				settings = &settings::legit_bot::legitbot_items[settings_weap];

				current_weapon_checkbox();
			}

			ImGui::Checkbox("bind##legit_aimbot", &settings::legit_bot::bind_check);
			ImGui::SameLine();
			ImGui::KeyBind("##enable_legit_aimbot", &settings::legit_bot::bind, ImGuiKeyBindFlags_OnItem);
		}
		ImGui::EndChild("weapon select");

		ImGui::BeginChild("general##legit", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::Checkbox("enabled", &settings->enabled);

			ImGui::Checkbox("friendly fire", &settings->deathmatch);

			if (settings::legit_bot::settings_category == 0)
				ImGui::Checkbox("auto pistol", &settings->autopistol);

			else if (settings::legit_bot::settings_category == 1)
			{
				if (settings::legit_bot::weapon_group == 1)
				{
					ImGui::Checkbox("auto pistol", &settings->autopistol);
				}
			}

			else if (settings::legit_bot::settings_category == 2)
			{
				if (settings_weap == WEAPON_P250 ||
					settings_weap == WEAPON_USP_SILENCER ||
					settings_weap == WEAPON_GLOCK ||
					settings_weap == WEAPON_FIVESEVEN ||
					settings_weap == WEAPON_TEC9 ||
					settings_weap == WEAPON_DEAGLE ||
					settings_weap == WEAPON_ELITE ||
					settings_weap == WEAPON_HKP2000) {
					ImGui::Checkbox("auto pistol", &settings->autopistol);
				}
			}

			const char* check_c[4] = { "smoke", "flash", "jump", "zoom" };
			bool* check_b[4] = { &settings->smoke_check, &settings->flash_check, &settings->jump_check, &settings->only_in_zoom };
			ImGui::MultiCombo("check", check_c, check_b, 4);

			if (settings->flash_check)
				ImGui::SliderInt("min alpha##flash_check", &settings->flash_check_min, 0, 255);
			ImGui::Checkbox("autowall", &settings->autowall);
			ImGui::Checkbox("enable override hitbox##check", &settings->enable_hitbox_override);
			ImGui::SameLine();
			ImGui::KeyBind("##bind_override_hitbox", &settings->hitbox_override_bind);
			ImGui::MultiCombo("override hitboxes", hitbox_list, settings->override_hitboxes, 7);
		}
		ImGui::EndChild("general##legit");
		ImGui::NextColumn();
		ImGui::BeginChild("weapon settings", ImVec2(0, 200), true, ImGuiWindowFlags_ChildWindowTitle);
		{
			ImGui::MultiCombo("hitboxes", hitbox_list, settings->hitboxes, 7);

			ImGui::Combo("smooth type", &settings->smooth_type, smooth_types, IM_ARRAYSIZE(smooth_types));
			ImGui::SliderFloat("basic fov", &settings->fov, 0, 30, "%.2f");
			ImGui::SliderFloat("smooth", &settings->smooth, 1, 20, "%.2f");
			ImGui::SliderInt("kill delay", &settings->kill_delay, 0, 1000, "%d sec");
			ImGui::SliderFloat("silent fov", &settings->silent_fov, 0, 20, "%.2f");
			ImGui::SliderInt("silent chance", &settings->silent_chance, 0, 100, "%i %%");


			if (settings->autowall) {
				ImGui::SliderInt("min damage##aimbot.min_damage", &settings->min_damage, 1, 100);
			}

			ImGui::SliderInt("backtrack tick [?]", &settings->backtrack_time, 0, 12, "%i ticks");
			if (ImGui::ItemsToolTipBegin("##backtracktooltip"))
			{
				ImGui::Checkbox("aim at backtrack", &settings->aim_at_backtrack);
				ImGui::ItemsToolTipEnd();
			}

		}
		ImGui::EndChild("weapon settings");

		ImGui::BeginChild("rcs setting", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
		{

			ImGui::Checkbox("enable", &settings->rcs);
			static char* rcs_types[] = {
				"standalone",
				"aim"
			};
			ImGui::Combo("rcs type", &settings->rcs_type, rcs_types, IM_ARRAYSIZE(rcs_types));
			ImGui::Checkbox("custom Fov", &settings->rcs_fov_enabled);

			if (settings->rcs_fov_enabled)
			{
				ImGui::SliderFloat("rcs fov", &settings->rcs_fov, 0, 20, "%.2f *");
			}

			ImGui::Checkbox("custom smooth", &settings->rcs_smooth_enabled);

			if (settings->rcs_smooth_enabled)
			{
				ImGui::SliderFloat("rcs smooth", &settings->rcs_smooth, 1, 15);
			}

			ImGui::SliderInt("rcs x", &settings->rcs_x, 0, 100, "%i %%");
			ImGui::SliderInt("rcs y", &settings->rcs_y, 0, 100, "%i %%");
			ImGui::SliderInt("rcs start bullet", &settings->rcs_start, 1, 30);
		}
		ImGui::EndChild("rcs setting");


	}
}