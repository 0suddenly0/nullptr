#include "../hooks.h"

namespace hooks
{
	void impact::hook()
	{
		for (auto head = g_effects_head; head; head = head->next)
		{
			if (strstr(head->effectName, "Impact") && strlen(head->effectName) <= 8)
			{
				o_impact = head->function;
				head->function = &visuals::draw_impacts;
			    break;
			}
		}
	}

	void impact::destroy()
	{
		for (auto head = g_effects_head; head; head = head->next)
		{
			if (strstr(head->effectName, "Impact") && strlen(head->effectName) <= 8) {
				head->function = o_impact;
				break;
			}
		}
	}
}