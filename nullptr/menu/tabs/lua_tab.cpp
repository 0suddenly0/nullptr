#include "../menu.h"

struct lua_func_t
{
	std::string name_lua;
	std::vector<menu_item_t> items;
};

constexpr auto& all_lua_loaded = lua::scripts_loaded;
constexpr auto& all_lua = lua::scripts_can_load;
static int current_lua_id = -1;
static std::string current_lua_name = "";

static int current_lua_id_loaded = -1;
static std::string current_lua_name_loaded = "";


void restore()
{
	current_lua_id = -1;
	current_lua_name = "";
}

void restore_loaded()
{
	current_lua_id_loaded = -1;
	current_lua_name_loaded = "";
}

namespace menu
{
	void tabs::lua_tab()
	{
		if (static_cast<size_t>(current_lua_id) >= all_lua.size())
			restore();

		if (static_cast<size_t>(current_lua_id_loaded) >= all_lua_loaded.size())
			restore_loaded();

		if (general_tab_lua == 0)
		{
			ImGui::Columns(2, nullptr, false);
			ImGui::BeginChild("lua list##lua", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				ImGui::ListBoxHeader("lua##lua", ImVec2(0, 286));
				{
					for (int i = 0; i < all_lua.size(); i++)
					{
						if (ImGui::Selectable(all_lua[i].substr(0, all_lua[i].length() - 4).c_str(), current_lua_name == all_lua[i])) {
							current_lua_name = all_lua[i];
							current_lua_id = i;
						}
					}
				}
				ImGui::ListBoxFooter();

				if (ImGui::ButtonS("refresh", ImVec2(221, 0)))
				{
					if (settings::visuals::events::enable)
					{
						if (settings::visuals::events::lua.enable_screen)
							notify::add("lua list refreshed", 7500);
						if (settings::visuals::events::lua.enable_console)
							notify::add("lua", "lua list refreshed", settings::visuals::events::lua.custom_color ? settings::visuals::events::lua.color : settings::misc::menu_color, log_type::console);
					}

					restore();

					lua::unload_all_scripts();
					lua::refresh_scripts();
				}

				if (current_lua_id != -1)
				{
					if (!lua::loaded[lua::get_script_id(current_lua_name)])
					{
						if (ImGui::ButtonS("load selected", ImVec2(221, 0)))
						{
							if (settings::visuals::events::enable)
							{
								if (settings::visuals::events::lua.enable_screen)
									notify::add("lua - " + current_lua_name.substr(0, current_lua_name.length() - 4) + " loaded", 7500);
								if (settings::visuals::events::lua.enable_console)
									notify::add("lua", "lua - " + current_lua_name.substr(0, current_lua_name.length() - 4) + " loaded", settings::visuals::events::lua.custom_color ? settings::visuals::events::lua.color : settings::misc::menu_color, log_type::console);
							}

							lua::load_script(current_lua_name);
						}
					}

				}
			}
			ImGui::EndChild("lua list##lua");

			ImGui::NextColumn();

			ImGui::BeginChild("loaded lua list##lua", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
			{
				ImGui::ListBoxHeader("lua loaded##lua", ImVec2(0, 286));
				{
					for (int i = 0; i < all_lua_loaded.size(); i++)
					{
						if (ImGui::Selectable(all_lua_loaded[i].substr(0, all_lua_loaded[i].length() - 4).c_str(), current_lua_name_loaded == all_lua_loaded[i])) {
							current_lua_name_loaded = all_lua_loaded[i];
							current_lua_id_loaded = i;
						}
					}
				}
				ImGui::ListBoxFooter();

				if (ImGui::ButtonS("unload all", ImVec2(108.5, 0)))
				{
					if (settings::visuals::events::enable)
					{
						if (settings::visuals::events::lua.enable_screen)
							notify::add("all lua unloaded", 7500);
						if (settings::visuals::events::lua.enable_console)
							notify::add("lua", "all lua unloaded", settings::visuals::events::lua.custom_color ? settings::visuals::events::lua.color : settings::misc::menu_color, log_type::console);
					}

					restore_loaded();

					lua::unload_all_scripts();
				}

				ImGui::SameLine();

				if (ImGui::ButtonS("reload all", ImVec2(108.5, 0)))
				{
					if (settings::visuals::events::enable)
					{
						if (settings::visuals::events::lua.enable_screen)
							notify::add("all lua reloaded", 7500);
						if (settings::visuals::events::lua.enable_console)
							notify::add("lua", "all lua reloaded", settings::visuals::events::lua.custom_color ? settings::visuals::events::lua.color : settings::misc::menu_color, log_type::console);
					}

					restore();

					lua::reload_all_scripts();
				}

				if (lua::get_script_id(current_lua_name_loaded) != -1)
				{
					if (lua::loaded[lua::get_script_id(current_lua_name_loaded)])
					{
						if (ImGui::ButtonS("unload selected", ImVec2(108.5, 0)))
						{
							if (settings::visuals::events::enable)
							{
								if (settings::visuals::events::lua.enable_screen)
									notify::add("lua - " + current_lua_name_loaded.substr(0, current_lua_name_loaded.length() - 4) + " unloaded", 7500);
								if (settings::visuals::events::lua.enable_console)
									notify::add("lua", "lua - " + current_lua_name_loaded.substr(0, current_lua_name_loaded.length() - 4) + " unloaded", settings::visuals::events::lua.custom_color ? settings::visuals::events::lua.color : settings::misc::menu_color, log_type::console);
							}

							lua::unload_script(lua::get_script_id(current_lua_name_loaded));
						}

						ImGui::SameLine();

						if (ImGui::ButtonS("reload selected", ImVec2(108.5, 0)))
						{
							if (settings::visuals::events::enable)
							{
								if (settings::visuals::events::lua.enable_screen)
									notify::add("lua - " + current_lua_name_loaded.substr(0, current_lua_name_loaded.length() - 4) + " reloaded", 7500);
								if (settings::visuals::events::lua.enable_console)
									notify::add("lua", "lua - " + current_lua_name_loaded.substr(0, current_lua_name_loaded.length() - 4) + " reloaded", settings::visuals::events::lua.custom_color ? settings::visuals::events::lua.color : settings::misc::menu_color, log_type::console);
							}

							lua::unload_script(lua::get_script_id(current_lua_name_loaded));
							lua::load_script(lua::get_script_id(current_lua_name_loaded));
						}
					}
				}

			}
			ImGui::EndChild("loaded lua list##lua");
		}
		else
		{
			ImGui::Columns(2, nullptr, false);

			std::vector<lua_func_t> left_colum;
			std::vector<lua_func_t> right_colum;

			for (int i = 0; i < lua::scripts_loaded_with_gui_elemets.size(); i++)
			{
				auto list = lua::get_all_items_on_script(lua::get_script_id(lua::scripts_loaded_with_gui_elemets[i]));
				if (list.size() != 0)
				{
					lua_func_t func;
					func.name_lua = lua::scripts_loaded_with_gui_elemets[i];
					func.items = list;

					if (i % 2 == 0.0f) left_colum.push_back(func);
					else right_colum.push_back(func);
				}
			}

			for (int i = 0; i < left_colum.size(); i++)
			{
				ImGui::BeginChild(left_colum[i].name_lua.c_str(), ImVec2(0, lua::get_group_size(left_colum[i].items)), true, ImGuiWindowFlags_ChildWindowTitle);
				{
					for (int item = 0; item < left_colum[i].items.size(); item++)
					{
						auto* item_cur = &left_colum[i].items[item];
						switch (item_cur->type)
						{
						case menuitem_button:
							GET_BOOL[item_cur->var] = ImGui::Button(std::string(item_cur->label + "##" + left_colum[i].name_lua.c_str()).c_str());
							break;
						case menuitem_checkbox:
							ImGui::Checkbox(std::string(item_cur->label + "##" + left_colum[i].name_lua.c_str()).c_str(), &(GET_BOOL[item_cur->var]));
							break;
						case menuitem_sliderint:
							ImGui::SliderInt(std::string(item_cur->label + "##" + left_colum[i].name_lua.c_str()).c_str(), &(GET_INT[item_cur->var]), item_cur->sliderint_min, item_cur->sliderint_max);
							break;
						case menuitem_sliderfloat:
							ImGui::SliderFloat(std::string(item_cur->label + "##" + left_colum[i].name_lua.c_str()).c_str(), &(GET_FLOAT[item_cur->var]), item_cur->sliderfloat_min, item_cur->sliderfloat_max);
							break;
						}
					}
				}
				ImGui::EndChild(left_colum[i].name_lua.c_str());
			}

			ImGui::NextColumn();

			for (int i = 0; i < right_colum.size(); i++)
			{
				ImGui::BeginChild(right_colum[i].name_lua.c_str(), ImVec2(0, lua::get_group_size(right_colum[i].items)), true, ImGuiWindowFlags_ChildWindowTitle);
				{
					for (int item = 0; item < right_colum[i].items.size(); item++)
					{
						auto* item_cur = &right_colum[i].items[item];
						switch (item_cur->type)
						{
						case menuitem_button:
							GET_BOOL[item_cur->var] = ImGui::Button(std::string(item_cur->label + "##" + right_colum[i].name_lua.c_str()).c_str());
							break;
						case menuitem_checkbox:
							ImGui::Checkbox(std::string(item_cur->label + "##" + right_colum[i].name_lua.c_str()).c_str(), &(GET_BOOL[item_cur->var]));
							break;
						case menuitem_sliderint:
							ImGui::SliderInt(std::string(item_cur->label + "##" + left_colum[i].name_lua.c_str()).c_str(), &(GET_INT[item_cur->var]), item_cur->sliderint_min, item_cur->sliderint_max);
							break;
						}
					}
				}
				ImGui::EndChild(right_colum[i].name_lua.c_str());
			}
		}
	}
}