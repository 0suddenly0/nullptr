#include "../hooks.h"

namespace hooks
{
	void __stdcall frame_stage_notify::hook(frame_stage framestage)
	{
		auto frameStage = client_vhook.get_original< fn >(index::frame_stage_notify);

		if (!g_client_state || !g_local_player || !g_engine_client->is_connected())
		{
			if (netchannel_vhook.class_base)
			{
				netchannel_vhook.class_clear();
			}
		}
		else if (!g_client_state->net_channel && netchannel_vhook.class_base)
		{
			netchannel_vhook.class_clear();
		}

		if (!g_local_player)
		{
			misc::fake_latency::clear_sequence();
		}

		if (framestage == frame_stage::NET_UPDATE_POSTDATAUPDATE_START)
		{
			misc::fake_latency::update_sequence();
		}

		return frameStage(g_chl_client, framestage);
	}
}