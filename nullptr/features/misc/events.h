#pragma once
#include "misc.h"

class c_events : public i_game_event_listener2
{
public:
	void init();
	void shutdown();

private:
	virtual void fire_game_event(i_game_event* event);
	virtual int  get_event_debug_id(void);
};

extern c_events events;