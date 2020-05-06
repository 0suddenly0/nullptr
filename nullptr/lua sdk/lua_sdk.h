#pragma once
#include <map>
#include "sol.hpp"
#include <filesystem>
#include "../valve_sdk/misc/Color.h"

#define GET_BOOL lua::lua_config::b
#define GET_INT lua::lua_config::i
#define GET_FLOAT lua::lua_config::f
#define GET_COLOR lua::lua_config::c
#define GET_STRING lua::lua_config::strings

enum menuitem_type {
	menuitem_checkbox = 0,
	menuitem_sliderint,
	menuitem_sliderfloat,
	menuitem_keybind,
	menuitem_text,
	menuitem_combo,
	menuitem_multicombo,
	menuitem_colorpicker,
	menuitem_button
};

struct menu_item_t 
{
	menuitem_type type;
	std::string label = "";
	std::string var = "";

	int lua_id = -1;
	
	int sliderint_min = 0;
	int sliderint_max = 1;

	float sliderfloat_min = 0.f;
	float sliderfloat_max = 1.f;

	// defaults
	bool b_default = false;
	int i_default = 0;
	float f_default = 0.f;
	Color c_default = Color(255,255,255,255);

	// callbacks
	sol::function callback;
};

namespace lua {

	namespace lua_config
	{
		extern std::unordered_map<std::string, bool> b;
		extern std::unordered_map<std::string, int> i;
		extern std::unordered_map<std::string, float> f;
		extern std::unordered_map<std::string, float[4]> c;
		extern std::unordered_map<std::string, std::string> strings;
	}

	struct c_lua_hook {
		int scriptId;
		sol::protected_function func;
	};

	class c_lua_hook_manager {
	public:
		void register_hook(std::string eventName, int scriptId, sol::protected_function func);
		void unregister_hooks(int scriptId);

		std::vector<c_lua_hook> get_hooks(std::string eventName);

	private:
		std::map<std::string, std::vector<c_lua_hook>> hooks;
	};

	extern std::vector<menu_item_t> lua_menu_items;

	std::vector<menu_item_t> get_all_items_on_script(int id);
	void init_state();
	void init_command();
	void unload();

	void load_script(int id);
	void load_script(std::string name);
	void unload_script(int id);
	void reload_all_scripts();
	void unload_all_scripts();
	
	void refresh_scripts();
	int get_script_id(std::string name);
	int get_script_id_in_loaded(std::string name);
	std::string get_script_name(int id);
	int get_script_id_by_path(std::string path);
	std::string get_script_path(std::string name);
	std::string get_script_path(int id);
	
	float get_group_size(std::vector<menu_item_t> items);

	extern std::vector<std::filesystem::path> pathes;
	extern std::vector<bool> loaded;
	extern std::vector<std::string> scripts_can_load;
	extern std::vector<std::string> scripts_loaded;
	extern std::vector<std::string> scripts_loaded_with_gui_elemets;
	extern std::vector<std::string> scripts;
	extern c_lua_hook_manager* hooks;
	extern lua_State* g_lua_state;
	extern bool g_unload_flag;
}
