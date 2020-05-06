#pragma once
#include <string>
#include <deque>
#include "../../render/render.h"
#include "../../valve_sdk/sdk.h"
#include "../../options/options.h"

/*namespace notify_old
{
	void notify_events(i_game_event* event);
	void render();

	namespace console
	{
		void clear();

		void notify(std::string pre, std::string msg, Color clr_pre = settings::misc::menu_color, Color clr_msg = Color(255, 255, 255, 255));
	}
	namespace screen
	{
		void notify(std::string pre, std::string text, Color color_pre = settings::misc::menu_color, Color color_text = Color(255, 255, 255, 255), int life_time = 4700);
	}
}*/

enum log_type
{
	console = 0,
	screen_standart = 1,
	screen_big = 2
};

namespace notify
{
	void add(std::string pre, std::string body, Color color_pre, Color color_body, int life_time, log_type type);
	void add(std::string pre, std::string body, Color color_pre, int life_time, log_type type);
	void add(std::string pre, std::string body, Color color_pre, log_type type);
	void add(std::string pre, std::string body, log_type type);
	void add(std::string pre, std::string body, Color color_pre, Color color_body = Color(255,255,255,255));
	void add(std::string pre, std::string body, int life_time);
	void add(std::string pre, std::string body);
	void add(std::string body, int life_time);
	void add(std::string body);

	void events(i_game_event* event);
	void render();
}