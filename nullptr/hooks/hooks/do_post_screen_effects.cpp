#include "../hooks.h"

namespace hooks
{
	int __stdcall do_post_screen_effects::hook(int a1)
	{
		auto oDoPostScreenEffects = clientmode_vhook.get_original<fn>(index::do_post_screen_space_effects);

		if (g_local_player)
		{
			glow::init();
		}

		return oDoPostScreenEffects(g_client_mode, a1);
	}
}