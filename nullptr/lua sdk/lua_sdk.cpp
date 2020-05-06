#include <fstream>
#include <ShlObj.h>
#include "lua_sdk.h"
#include "../options/options.h"
#include "../valve_sdk/sdk.h"
#include "lauxlib.h"
#include "../render/render.h"

int extract_owner(sol::this_state st) {
	sol::state_view lua_state(st);
	sol::table rs = lua_state["debug"]["getinfo"](2, "S");
	std::string source = rs["source"];
	std::string filename = std::filesystem::path(source.substr(1)).filename().string();
	return lua::get_script_id(filename);
}

namespace lua_gui
{
	std::tuple<sol::object, sol::object, sol::object, sol::object> get(sol::this_state s, std::string key) {
		std::tuple<sol::object, sol::object, sol::object, sol::object> retn = std::make_tuple(sol::nil, sol::nil, sol::nil, sol::nil);

		for (auto kv : lua::lua_config::b)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

		for (auto kv : lua::lua_config::c)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, (int)(kv.second[0] * 255)), sol::make_object(s, (int)(kv.second[1] * 255)), sol::make_object(s, (int)(kv.second[2] * 255)), sol::make_object(s, (int)(kv.second[3] * 255)));

		for (auto kv : lua::lua_config::f)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);


		for (auto kv : lua::lua_config::i)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

		for (auto kv : lua::lua_config::strings)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

		return retn;
	}

	void add_button(sol::this_state s, std::string name, std::string var)
	{
		menu_item_t item;
		item.type = menuitem_type::menuitem_button;
		item.label = name;
		item.var = var;
		item.lua_id = extract_owner(s);

		lua::lua_menu_items.push_back(item);
	}

	void add_checkbox(sol::this_state s, std::string name, std::string var)
	{
		menu_item_t item;
		item.type = menuitem_type::menuitem_checkbox;
		item.label = name;
		item.var = var;
		item.lua_id = extract_owner(s);

		lua::lua_menu_items.push_back(item);
	}

	void add_sliderint(sol::this_state s, std::string name, std::string var, int min, int max)
	{
		menu_item_t item;
		item.type = menuitem_type::menuitem_sliderint;
		item.label = name;
		item.var = var;
		item.lua_id = extract_owner(s);

		item.sliderint_min = min;
		item.sliderint_max = max;

		lua::lua_menu_items.push_back(item);
	}

	void add_sliderfloat(sol::this_state s, std::string name, std::string var, float min, float max)
	{
		menu_item_t item;
		item.type = menuitem_type::menuitem_sliderfloat;
		item.label = name;
		item.var = var;
		item.lua_id = extract_owner(s);

		item.sliderfloat_min = min;
		item.sliderfloat_max = max;

		lua::lua_menu_items.push_back(item);
	}
}

namespace lua_client
{
	void set_event_callback(sol::this_state s, std::string eventname, sol::function func) {
		sol::state_view lua_state(s);
		sol::table rs = lua_state["debug"]["getinfo"](2, ("S"));
		std::string source = rs["source"];
		std::string filename = std::filesystem::path(source.substr(1)).filename().string();

		lua::hooks->register_hook(eventname, lua::get_script_id(filename), func);
	}

	int get_netvar_offset(std::string table, std::string netvar)
	{
		return netvar_sys::get_offset(table, netvar);
	}

	std::string get_cheat_version()
	{
		return utils::snprintf("%.2f", NULLPTR_VERSION);
	}

	void unload(std::string name)
	{
		lua::unload_script(lua::get_script_id(name));
	}

	void run_script(std::string scriptname) 
	{
		lua::load_script(lua::get_script_id(scriptname));
	}

	void reload_active_scripts() {
		lua::reload_all_scripts();
	}

	void refresh() {
		lua::unload_all_scripts();
		lua::refresh_scripts();
	}
}

namespace lua_entity_list
{
	int get_highest_entity_index()
	{
		return g_entity_list->get_highest_entity_index();
	}

	c_base_entity* get_entity_by_index(int idx)
	{
		return (c_base_entity*)g_entity_list->get_client_entity(idx);
	}
}

namespace lua_engine_client
{
	void print_console(std::string text)
	{
		g_cvar->console_dprintf(text.c_str());
	}

	int get_max_players()
	{
		return g_engine_client->get_max_clients();
	}

	int get_local_player()
	{
		return g_engine_client->get_local_player();
	}

	void console_command(std::string text)
	{
		g_engine_client->execute_client_cmd(text.c_str());
	}

	Vector get_screen_size()
	{
		int w, h;
		g_engine_client->get_screen_size(w, h);

		return Vector{ (float)w, (float)h, 0.f };
	}

	bool is_connected()
	{
		return g_engine_client->is_connected();
	}

	bool is_ingame()
	{
		return g_engine_client->is_ingame();
	}
}

namespace lua_render
{
	void draw_text(std::string text, int x, int y, Color col, int size)
	{
		render::draw_text(text, x, y, size, col);
	}

	void draw_box(int x, int y, int x_end, int y_end)
	{
		render::draw_box(x, y, x_end, y_end, Color(255, 255, 255, 255));
	}

	void draw_rect_filled(int x, int y, int x_end, int y_end, Color col)
	{
		render::draw_box_filled(x, y, x_end, y_end, col);
	}

	void draw_box_multicolor(int x, int y, int x_end, int y_end, Color left, Color right)
	{

	}
	
}

namespace lua_cvar {
	convar* find_var(std::string name)
	{
		return g_cvar->find_var(name.c_str());
	}
}

namespace lua {
	bool g_unload_flag = false;
	lua_State* g_lua_state = NULL;
	c_lua_hook_manager* hooks = new c_lua_hook_manager();
	std::vector<bool> loaded;
	std::vector<std::string> scripts;
	std::vector<std::string> scripts_can_load;
	std::vector<std::string> scripts_loaded;
	std::vector<std::string> scripts_loaded_with_gui_elemets;
	std::vector<std::filesystem::path> pathes;
	std::vector<menu_item_t> lua_menu_items;

	namespace lua_config
	{
		std::unordered_map<std::string, bool> b = {};
		std::unordered_map<std::string, int> i = {};
		std::unordered_map<std::string, float> f = {};
		std::unordered_map<std::string, float[4]> c = {};
		std::unordered_map<std::string, std::string> strings = {};
	}

	int extract_owner(sol::this_state st) {
		sol::state_view lua_state(st);
		sol::table rs = lua_state["debug"]["getinfo"](2, "S");
		std::string source = rs["source"];
		std::string filename = std::filesystem::path(source.substr(1)).filename().string();
		return get_script_id(filename);
	}

	void init_state() {
#ifdef _DEBUG
		lua_writestring(LUA_COPYRIGHT, strlen(LUA_COPYRIGHT));
		lua_writeline();
#endif
		lua::unload();
		g_lua_state = luaL_newstate();
		luaL_openlibs(g_lua_state);
	}

	void init_command() {
		sol::state_view lua_state(g_lua_state);
		lua_state["exit"] = []() { g_unload_flag = true; };

		lua_state.new_enum("buttons",
			"in_attack", IN_ATTACK,
			"in_attack2", IN_ATTACK2,
			"in_jump", IN_JUMP,
			"in_duck", IN_DUCK,
			"in_forward", IN_FORWARD,
			"in_back", IN_BACK,
			"in_use", IN_USE,
			"in_move_left", IN_MOVELEFT,
			"in_move_right", IN_MOVERIGHT,
			"in_score", IN_SCORE,
			"in_bullrush", IN_BULLRUSH);

		lua_state.new_usertype<Vector2D>("vector2d",
			sol::constructors<Vector2D(), Vector2D(int, int), Vector2D(float, float)>(),
			"x", &Vector2D::x,
			"y", &Vector2D::y
			);

		lua_state.new_usertype<Vector>("vector",
			sol::constructors<Vector(), Vector(int, int, int), Vector(float, float, float)>(),
			"x", &Vector::x,
			"y", &Vector::y,
			"z", &Vector::z
			);

		lua_state.new_usertype<Color>("color",
			sol::constructors<Color(), Color(int, int, int), Color(int, int, int, int)>(),
			"r", &Color::r,
			"g", &Color::g,
			"b", &Color::b,
			"a", &Color::a,
			"from_hsb", &Color::FromHSB,
			"from_hsv", &Color::FromHSV
			);

		lua_state.new_usertype<c_base_entity>("base_entity",
			"is_player", &c_base_entity::is_player,
			"get_health", &c_base_entity::get_health,

			"set_netvar_val", sol::overload(&c_base_entity::set_lua_netvar_int,
				&c_base_entity::set_lua_netvar_float,
				&c_base_entity::set_lua_netvar_bool),

			"get_netvar_val", sol::overload(&c_base_entity::get_lua_netvar_int,
				&c_base_entity::get_lua_netvar_float,
				&c_base_entity::get_lua_netvar_bool)
			);

		lua_state.new_usertype<convar>("convar",
			"get_int", &convar::get_int,
			"get_float", &convar::get_float,
			"get_string", &convar::get_string,
			"get_bool", &convar::get_bool,
			"get_color", &convar::get_color,

			"set_int", & convar::set_value_int,
			"set_float", & convar::set_value_float,
			"set_string", & convar::set_value_char,
			"set_color", & convar::set_value_color
			);

		auto m_entity_list = lua_state.create_table();
		m_entity_list["get_highest_entity_index"] = lua_entity_list::get_highest_entity_index;
		m_entity_list["get_entity_by_index"] = lua_entity_list::get_entity_by_index;
		lua_state["entity_list"] = m_entity_list;

		auto m_gui = lua_state.create_table();
		m_gui["get"] = lua_gui::get;
		m_gui["add_button"] = lua_gui::add_button;
		m_gui["add_checkbox"] = lua_gui::add_checkbox;
		m_gui["add_sliderint"] = lua_gui::add_sliderint;
		m_gui["add_sliderfloat"] = lua_gui::add_sliderfloat;
		lua_state["gui"] = m_gui;

		auto m_cvar = lua_state.create_table();
		m_cvar["find_var"] = lua_cvar::find_var;
		lua_state["cvar"] = m_cvar;

		auto m_engine = lua_state.create_table();
		m_engine["get_screen_size"] = lua_engine_client::get_screen_size;
		m_engine["print_console"] = lua_engine_client::print_console;
		m_engine["console_command"] = lua_engine_client::console_command;
		m_engine["get_max_players"] = lua_engine_client::get_max_players;
		m_engine["get_local_player"] = lua_engine_client::get_local_player;
		m_engine["is_connected"] = lua_engine_client::is_connected;
		m_engine["is_ingame"] = lua_engine_client::is_ingame;
		lua_state["engine_client"] = m_engine;


		auto m_client = lua_state.create_table();
		m_client["get_netvar_offset"] = lua_client::get_netvar_offset;
		m_client["unload_lua"] = lua_client::unload;
		m_client["set_func_callback"] = lua_client::set_event_callback;
		m_client["run_lua"] = lua_client::run_script;
		m_client["reload_active_lua"] = lua_client::reload_active_scripts;
		m_client["refresh_lua_list"] = lua_client::refresh;
		lua_state["client"] = m_client;

		auto m_render = lua_state.create_table();
		m_render["draw_text"] = lua_render::draw_text;
		m_render["draw_rect_filled"] = lua_render::draw_rect_filled;
		lua_state["render"] = m_render;
		
		
		refresh_scripts();
	}


	void unload() {
		if (g_lua_state != NULL) {
			lua_close(g_lua_state);
			g_lua_state = NULL;
		}
	}

	void load_script(int id) {
		if (id == -1)
			return;

		if (loaded.at(id))
			return;

		auto path = get_script_path(id);
		if (path == (""))
			return;

		sol::state_view state(g_lua_state);
		state.script_file(path, [](lua_State* me, sol::protected_function_result result) {
			if (!result.valid()) {
				sol::error err = result;
				//Utilities->Game_Msg(err.what());
				//printf("%s\n", err.what());
			}

			return result;
			});
		loaded.at(id) = true;
		scripts_loaded.push_back(get_script_name(id));
		if(get_all_items_on_script(id).size() > 0)
			scripts_loaded_with_gui_elemets.push_back(get_script_name(id));

		for (int i = 0; i < scripts_can_load.size(); i++)
		{
			if (scripts_can_load[i] == get_script_name(id))
			{
				scripts_can_load.erase(scripts_can_load.begin() + i);
			}
		}
	}


	void load_script(std::string name) {
		if (get_script_id(name) == -1)
			return;

		if (loaded.at(get_script_id(name)))
			return;

		auto path = get_script_path(get_script_id(name));
		if (path == (""))
			return;

		sol::state_view state(g_lua_state);
		state.script_file(path, [](lua_State* me, sol::protected_function_result result) {
			if (!result.valid()) {
				sol::error err = result;
				//Utilities->Game_Msg(err.what());
				//printf("%s\n", err.what());
			}

			return result;
			});
		loaded.at(get_script_id(name)) = true;
		scripts_loaded.push_back(name);
		if (get_all_items_on_script(get_script_id(name)).size() > 0)
			scripts_loaded_with_gui_elemets.push_back(name);

		for (int i = 0; i < scripts_can_load.size(); i++)
		{
			if (scripts_can_load[i] == name)
			{
				scripts_can_load.erase(scripts_can_load.begin() + i);
			}
		}
	}

	void unload_script(int id) {
		if (id == -1)
			return;

		if (!loaded.at(id))
			return;

		std::vector<menu_item_t> updated_items;

		for (int i = 0; i < lua_menu_items.size(); i++)
		{
			if (lua_menu_items[i].lua_id != id)
				updated_items.push_back(lua_menu_items[i]);
		}

		lua_menu_items = updated_items;

		hooks->unregister_hooks(id);
		loaded.at(id) = false;

		std::string name = get_script_name(id);

		for (int i = 0; i < scripts_loaded.size(); i++)
		{
			if (scripts_loaded[i] == name)
			{
				scripts_loaded.erase(scripts_loaded.begin() + i);
			}
		}

		for (int i = 0; i < scripts_loaded_with_gui_elemets.size(); i++)
		{
			if (scripts_loaded_with_gui_elemets[i] == name)
			{
				scripts_loaded_with_gui_elemets.erase(scripts_loaded_with_gui_elemets.begin() + i);
			}
		}

		scripts_can_load.push_back(get_script_name(id));
	}

	void reload_all_scripts() {
		for (auto s : scripts) {
			if (loaded.at(get_script_id(s))) {
				unload_script(get_script_id(s));
				load_script(get_script_id(s));
			}
		}
	}

	void unload_all_scripts() {
		for (auto s : scripts)
			if (loaded.at(get_script_id(s)))
				unload_script(get_script_id(s));
	}

	void refresh_scripts() 
	{
		CreateDirectoryA((LPCSTR)"C:\\nullptr\\lua", NULL);

		std::filesystem::path path;
		path = "C:\\nullptr\\lua";

		if (!std::filesystem::is_directory(path)) {
			std::filesystem::remove(path);
			std::filesystem::create_directory(path);
		}

		auto oldLoaded = loaded;
		auto oldScripts = scripts;

		scripts_loaded.clear();
		scripts_loaded_with_gui_elemets.clear();
		scripts_can_load.clear();
		loaded.clear();
		pathes.clear();
		scripts.clear();

		for (auto& entry : std::filesystem::directory_iterator((path))) {
			if (entry.path().extension() == (".lua")) {
				auto path = entry.path();
				auto filename = path.filename().string();

				bool didPut = false;
				int oldScriptsSize = 0;
				oldScriptsSize = oldScripts.size();
				if (oldScriptsSize < 0)
					continue;

				for (int i = 0; i < oldScriptsSize; i++) {
					if (filename == oldScripts.at(i)) {
						loaded.push_back(oldLoaded.at(i));
						didPut = true;
					}
				}

				if (!didPut)
					loaded.push_back(false);

				pathes.push_back(path);
				scripts.push_back(filename);
				scripts_can_load.push_back(filename);
			}
		}
	}

	float get_group_size(std::vector<menu_item_t> items)
	{
		float count = 33;

		for (int i = 0; i < items.size(); i++)
		{
			switch (items[i].type)
			{
			case menuitem_button:
				count += 16;
				break;
			case menuitem_checkbox:
				count += 16;
				break;
			case menuitem_sliderfloat:
				count += 24;
				break;
			case menuitem_sliderint:
				count += 24;
				break;
			}
			count += 4;
		}

		return count;
	}

	int get_script_id(std::string name) {
		int scriptsSize = 0;
		scriptsSize = scripts.size();
		if (scriptsSize <= 0)
			return -1;

		for (int i = 0; i < scriptsSize; i++) {
			if (scripts.at(i) == name)
				return i;
		}

		return -1;
	}

	int get_script_id_in_loaded(std::string name) 
	{
		int scriptsSize = 0;
		scriptsSize = scripts_loaded.size();
		if (scriptsSize <= 0)
			return -1;

		for (int i = 0; i < scriptsSize; i++) {
			if (scripts_loaded.at(i) == name)
				return i;
		}

		return -1;
	}

	int get_script_id_by_path(std::string path) {
		int pathesSize = 0;
		pathesSize = pathes.size();
		if (pathesSize <= 0)
			return -1;

		for (int i = 0; i < pathesSize; i++) {
			if (pathes.at(i).string() == path)
				return i;
		}

		return -1;
	}

	std::string get_script_path(std::string name) {
		return get_script_path(get_script_id(name));
	}

	std::string get_script_path(int id) {
		if (id == -1)
			return  "";

		return pathes.at(id).string();
	}

	std::string get_script_name(int id) {
		if (id == -1)
			return  "";

		return scripts.at(id);
	}

	std::vector<menu_item_t> get_all_items_on_script(int id)
	{
		std::vector<menu_item_t> ret_array;

		for (int i = 0; i < lua_menu_items.size(); i++)
		{
			if (lua_menu_items[i].lua_id == id)
			{
				ret_array.push_back(lua_menu_items[i]);
			}
		}

		return ret_array;
	}

	void c_lua_hook_manager::register_hook(std::string eventName, int scriptId, sol::protected_function func) {
		c_lua_hook hk = { scriptId, func };

		this->hooks[eventName].push_back(hk);
	}

	void c_lua_hook_manager::unregister_hooks(int scriptId) {
		for (auto& ev : this->hooks) {
			int pos = 0;

			for (auto& hk : ev.second) {
				if (hk.scriptId == scriptId)
					ev.second.erase(ev.second.begin() + pos);

				pos++;
			}
		}
	}

	std::vector<c_lua_hook> c_lua_hook_manager::get_hooks(std::string eventName) {
		return this->hooks[eventName];
	}
}

