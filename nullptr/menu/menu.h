#pragma once

#include <string>
#include "../options/options.h"
#include "../imgui/imgui.h"
#include "ui.h"
#include "../valve_sdk/csgostructs.h"
#include "../helpers/input.h"
#include "../render/render.h"
#include "../features/changers/skin/item_definitions.h"
#include "../features/changers/skin/kit_parser.h"
#include "../features/changers/skin/skins.h"
#include "../features/misc/misc.h"
#include "../options/options.h"
#include "../options/config.h"

#include "../lua sdk/lua_sdk.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

inline bind_info* find_bind_info(std::string name)
{
	for (int i = 0; i < globals::bind_window::binds.size(); i++)
	{
		if (globals::bind_window::binds[i]->name == name)
		{
			return globals::bind_window::binds[i];
		}
	}
	return NULL;
}
inline bind_info* create_bind_info(std::string name, bool state, bind_info_flags flag)
{
	bind_info* bind_find = find_bind_info(name);

	if (bind_find  != NULL)
	{
		bind_find->enable = state;
		return bind_find;
	}

	bind_info* bind_inf = new bind_info{ name, state, flag };

	globals::bind_window::binds.push_back(bind_inf);

	return bind_inf;
}

inline void delete_bind_info(std::string name)
{
	for (int i = 0; i < globals::bind_window::binds.size(); i++)
	{
		if (globals::bind_window::binds[i]->name == name)
		{
			globals::bind_window::binds.erase(globals::bind_window::binds.begin() + i);
		}
	}
}
template <class T>
inline void create_bind(std::string name, T enable, KeyBind_t* bind, bind_info_flags flag)
{
	if (enable)
	{
		bind_info* bindinfo = create_bind_info(name, bind->enable, flag);

		if (bind->bind_type == KeyBind_type::always)
		{
			bind->enable = true;
		}
		else if (bind->bind_type == KeyBind_type::toggle)
		{
			if (input_sys::was_key_pressed(bind->key_id))
			{
				bind->enable = !bind->enable;
			}
		}
	    else if (bind->bind_type == KeyBind_type::press)
		{
		    if (input_sys::is_key_down(bind->key_id))
		    {
			    bind->enable = true;
		    }

		    else if (!input_sys::is_key_down(bind->key_id))
		    {
			    bind->enable = false;
		    }
		}
		else if (bind->bind_type == KeyBind_type::press_invers)
		{
		    if (input_sys::is_key_down(bind->key_id))
		    {
		    	bind->enable = false;
		    }

		    else if (!input_sys::is_key_down(bind->key_id))
		    {
			    bind->enable = true;
		    }
		}

		if (bind->key_id == 0 && bind->bind_type != KeyBind_type::always)
			bind->enable = false;
	}
	else if (!enable)
	{
		delete_bind_info(name);
		bind->enable = false;
	}
}

template <class T>
inline void create_bind(std::string name, T enable, KeyBind_t* bind, float* side, bind_info_flags flag)
{
	if (enable)
	{
		bind_info* bindinfo = create_bind_info(name, bind->enable, flag);

		if (input_sys::was_key_pressed(bind->key_id))
		{
			bind->enable = !bind->enable;

			*side = -*side;
		}
	}
	else if (!enable)
	{
		delete_bind_info(name);
	}
}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h)
{
	bool values[N] = { false };
	values[activetab] = true;
	for (auto i = 0; i < N; ++i)
	{
		if (i == 0)
		{
			if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h }, 1))
				activetab = i;
		}
		else if (i == N - 1)
		{
			if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h }, 2))
				activetab = i;
		}
		else
		{
			if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h }, 0))
				activetab = i;
		}
		if (i < N - 1) ImGui::SameLine();
	}
}
template<size_t N>
void render_tabsMain(char* (&names)[N], int& activetab, float w, float h)
{
	bool values[N] = { false };
	values[activetab] = true;
	for (auto i = 0; i < N; ++i)
	{
		if (i == 0)
		{
			if (ImGui::ToggleButtonMain(names[i], &values[i], ImVec2{ w, h }, 1))
				activetab = i;
		}
		else if (i == N - 1)
		{
			if (ImGui::ToggleButtonMain(names[i], &values[i], ImVec2{ w, h }, 2))
				activetab = i;
		}
		else
		{
			if (ImGui::ToggleButtonMain(names[i], &values[i], ImVec2{ w, h }, 0))
				activetab = i;
		}
		if (i < N - 1) ImGui::SameLine();
	}
}

struct IDirect3DDevice9;

namespace menu
{
	std::string selected_weapon_name = "";
	std::string selected_skin_name = "";
	int definition_vector_index = 0;
	int general_tab = 0;
	int general_tab_lua = 0;

	int settings_weap;
	char localtag[100];

	namespace tabs
	{
		void lua_tab();
		void legit_tab();
		void visuals_tab();
		void misc_tab();
		void skins_tab();
		void profile_tab();
		void config_tab();
	}

	bool              _visible;

	void initialize();
	void shutdown();

	void on_device_lost();
	void on_device_reset();

	void render();

	void toggle();

	bool is_visible() { return _visible; }
	ImGuiStyle        _style;

	void create_style();
}