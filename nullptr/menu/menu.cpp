#include "menu.h"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "../security.h"
#include "../hooks/hooks.h"

void add_bind()
{
	create_bind("edge jump", settings::misc::edge_jump::enable, &settings::misc::edge_jump::bind, bind_info_flags_standart);
	create_bind("third person", settings::misc::third_person::enable, &settings::misc::third_person::bind, bind_info_flags_standart);
	create_bind("legit aa", settings::misc::desync::type, &settings::misc::desync::bind, &settings::misc::desync::side, bind_info_flags_side);
	create_bind("legit aim", settings::legit_bot::bind_check, &settings::legit_bot::bind, bind_info_flags_standart);
	create_bind("slowwalk", settings::misc::slowwalk::enable, &settings::misc::slowwalk::bind, bind_info_flags_standart);
	create_bind("block bot", settings::misc::block_bot::enable, &settings::misc::block_bot::bind, bind_info_flags_standart);
	create_bind("prepare revolver", settings::misc::prepare_revolver::enable, &settings::misc::prepare_revolver::bind, bind_info_flags_standart);

	create_bind("hitbox override", legit_aimbot::get_settings()->enable_hitbox_override, &legit_aimbot::get_settings()->hitbox_override_bind, bind_info_flags_standart);
}

void RenderBindWindow()
{
	ImGui::SetNextWindowPos(settings::windows::Bind_pos, ImGuiCond_Once);

	float becup = ImGui::GetStyle().Alpha;
	ImVec4 becup_windowbg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	ImVec4 becup_text = ImGui::GetStyle().Colors[ImGuiCol_Text];
	ImVec4 becup_titlebg = ImGui::GetStyle().Colors[ImGuiCol_TitleBg];
	ImVec4 becup_titlebar = ImGui::GetStyle().Colors[ImGuiCol_SliderGrab];

	bool window_enable = settings::misc::bind_window::enable && (globals::bind_window::binds.size() == 0 ? menu::is_visible() : true);

	if (window_enable)
	{
		ImGui::SetNextWindowBgAlpha(settings::windows::Bind_alpha);

		ImGui::GetStyle().Alpha = becup;
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = becup_windowbg;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = becup_text;
		ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = becup_titlebg;
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = becup_titlebar;
	}
	else
	{
		ImGui::SetNextWindowBgAlpha(0.f);
		ImGui::GetStyle().Alpha = 0.01f;
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.f, 0.f, 0.f, 0.f);
		ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(0.f, 0.f, 0.f, 0.f);
	}

	if (ImGui::Begin("bind's", nullptr, ImVec2(0, 0), settings::windows::Bind_alpha, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_RainbowTitleBar | ImGuiWindowFlags_NoCollapse | (window_enable ? 0 : ImGuiWindowFlags_NoMove)))
	{
		ImGui::GetCurrentWindow()->Hidden = !window_enable;

		settings::windows::Bind_pos = ImGui::GetWindowPos();

		ImGui::Columns(2, NULL, false);
		{
			ImGui::Text("bind name");
			ImGui::SetColumnWidth(-1, 130);
			ImGui::NextColumn();
			ImGui::Text("state");
		}
		ImGui::Columns(1, NULL, false);
		ImGui::Separator(170.f);

		ImGui::Columns(2, NULL, false);
		{
			for (int i = 0; i < globals::bind_window::binds.size(); i++)
			{
				auto& cur_bind = globals::bind_window::binds[i];

				ImGui::Text(cur_bind->name.c_str());

				ImGui::NextColumn();

				if (cur_bind->flag & bind_info_flags_standart)
					ImGui::Text(cur_bind->enable ? "enable" : "disable");
				if (cur_bind->flag & bind_info_flags_side)
					ImGui::Text(cur_bind->enable ? "left" : "right");

				ImGui::NextColumn();
			}
		}
		ImGui::Columns(1, NULL, false);
		ImGui::End();
	}

	ImGui::GetStyle().Alpha = becup;
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = becup_windowbg;
	ImGui::GetStyle().Colors[ImGuiCol_Text] = becup_text;
	ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = becup_titlebg;
	ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = becup_titlebar;

}

IDirect3DTexture9* m_skin_texture = nullptr;
std::string old_name_skin = "";
std::string old_name_weap = "";

namespace menu
{
	void initialize() {
		create_style();

		_visible = false;
	}

	void shutdown()
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void on_device_lost() 
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void on_device_reset() 
	{
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	void render()
	{
		ImGui::GetIO().MouseDrawCursor = _visible;

		add_bind();

		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(settings::misc::menu_color.r() / 255.f, settings::misc::menu_color.g() / 255.f, settings::misc::menu_color.b() / 255.f, 1.f);

		RenderBindWindow();

		visuals::draw_spectator_list();

		if (!_visible)
			return;

		static int selected_tab = 0;

		ImVec2 main_pos;
		ImVec2 main_size;

		ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2{ 600, 38 }, ImGuiSetCond_Once);

		float becup = ImGui::GetStyle().Alpha;
		ImVec2 becup_windowpd = ImGui::GetStyle().WindowPadding;

		ImGui::GetStyle().WindowPadding = ImVec2(0.f, 0.f);

		if (ImGui::Begin("N U L L P T R", &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_RainbowTitleBar))
		{
			main_pos = ImGui::GetWindowPos();
			main_size = ImGui::GetWindowSize();


			horizontal_tabs(selected_tab, tabs_c, true);

			ImGui::End();
		}

		ImGui::SetNextWindowPos(ImVec2(main_pos.x - 65, main_pos.y + main_size.y + 5.f));
		ImGui::SetNextWindowSize(ImVec2{ 60, 400 }, ImGuiSetCond_Once);

		if (selected_tab == 1 || selected_tab == 6)
		{
			if (ImGui::Begin("##N U L L P T R tabs", &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_RainbowTitleBar | ImGuiWindowFlags_NoTitleBar))
			{
				selected_tab == 1 ? vertical_tabs(general_tab, esp_tabs_general) : vertical_tabs(general_tab_lua, lua_tabs_general);

				ImGui::End();
			}
		}
		ImGui::GetStyle().WindowPadding = becup_windowpd;

		ImGui::SetNextWindowPos(ImVec2(main_pos.x, main_pos.y + main_size.y + 5.f));
		ImGui::SetNextWindowSize(ImVec2{ 600, 400 }, ImGuiSetCond_Once);

		if (ImGui::Begin("##N U L L P T R body", &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_RainbowTitleBar | ImGuiWindowFlags_NoTitleBar))
		{
			ImGui::BeginGroup();
			{
				switch (selected_tab)
				{
				case 0:
					tabs::legit_tab();
					break;
				case 1:
					tabs::visuals_tab();
					break;
				case 2:
					tabs::misc_tab();
					break;
				case 3:
					tabs::skins_tab();
					break;
				case 4:
					tabs::profile_tab();
					break;
				case 5:
					tabs::config_tab();
					break;
				case 6:
					tabs::lua_tab();
					break;
				}

				ImGui::EndGroup();
			}

			ImGui::End();
		}

		int definition_index = settings::changers::skin::m_items[k_weapon_names[definition_vector_index].definition_index].definition_index;
		int definition_override_vector_index = settings::changers::skin::m_items[k_weapon_names[definition_vector_index].definition_index].definition_override_vector_index;

		bool is_weapon = definition_index != GLOVE_T_SIDE &&
			definition_index != GLOVE_CT_SIDE &&
			definition_index != WEAPON_KNIFE &&
			definition_index != WEAPON_KNIFE_T;

		bool is_glove = definition_index == GLOVE_T_SIDE ||
			definition_index == GLOVE_CT_SIDE;

		bool is_knife = definition_index == WEAPON_KNIFE ||
			definition_index == WEAPON_KNIFE_T;

		bool is_ct = definition_index == WEAPON_KNIFE || definition_index == GLOVE_CT_SIDE;
		bool is_t = definition_index == WEAPON_KNIFE_T || definition_index == GLOVE_T_SIDE;

		auto& cur_gloves = is_ct ? k_ct_glove_names.at(definition_override_vector_index): k_t_glove_names.at(definition_override_vector_index);
		auto& cur_knifes = is_ct ? k_ct_knife_names.at(definition_override_vector_index) : k_t_knife_names.at(definition_override_vector_index);

		if (is_weapon)
		{
			selected_skin_name = k_weapon_names.at(definition_vector_index).skin_name.c_str();
			selected_weapon_name = k_weapon_names.at(definition_vector_index)._weapon_name.c_str();
		}
		else if (is_knife)
		{
			selected_skin_name = cur_knifes.skin_name.c_str();
			selected_weapon_name = cur_knifes._weapon_name.c_str();
		}
		else if (is_glove)
		{
			selected_skin_name = cur_gloves.skin_name.c_str();
			selected_weapon_name = cur_gloves._weapon_name.c_str();
		}

		bool skin_changed = selected_skin_name != old_name_skin;
		bool weapon_changed = selected_weapon_name != old_name_weap;
		bool standart = selected_skin_name == "" || selected_skin_name == "empty";

		bool dont_draw = selected_weapon_name == "default" || ((definition_vector_index == 2 || definition_vector_index == 3) && standart && (definition_override_vector_index != 1 && definition_override_vector_index != 2));

		if (settings::changers::skin::skin_preview && selected_tab == 3 && settings::changers::skin::show_cur)
		{
			if (skin_changed || weapon_changed)
			{
				std::string filename = standart ? "resource/flash/econ/weapons/base_weapons/" + std::string(selected_weapon_name) + ".png" : "resource/flash/econ/default_generated/" + std::string(selected_weapon_name) + "_" + std::string(selected_skin_name) + "_light_large.png";

				const auto handle = g_base_file_system->open(filename.c_str(), "r", "GAME");
				if (handle)
				{
					int file_len = g_base_file_system->size(handle);
					char* image = new char[file_len];

					g_base_file_system->read(image, file_len, handle);
					g_base_file_system->close(handle);

					D3DXCreateTextureFromFileInMemory(g_d3ddevice9, image, file_len, &m_skin_texture);

					delete[] image;

					old_name_skin = selected_skin_name;
					old_name_weap = selected_weapon_name;
				}
			}

			ImGui::SetNextWindowPos(ImVec2(main_pos.x + main_size.x + 5.f, main_pos.y + main_size.y + 5.f));
			ImGui::SetNextWindowSize(ImVec2{ 300, 250 });

			if (ImGui::Begin("skin preview##window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_RainbowTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{
				if (m_skin_texture && !dont_draw)
				{
					ImGui::Image(m_skin_texture, { 256, 192 });
				}

				ImGui::End();
			}
		}
	}

	void toggle()
	{
		_visible = !_visible;
	}

	void create_style()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigWindowsMoveFromTitleBarOnly = true;

		ImGui::StyleColorsDark();

		ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
		ImGui::GetStyle() = _style;
	}
}

