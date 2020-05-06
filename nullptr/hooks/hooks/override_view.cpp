#include "../hooks.h"

namespace hooks
{
	void __stdcall override_view::hook(c_view_setup* vsView)
	{
		static auto ofunc = clientmode_vhook.get_original<fn>(index::override_view);

		grenade_prediction::view();

		misc::no_smoke();

		if (g_engine_client->is_ingame() && vsView)
		{
			visuals::third_person();
		}
		if (g_local_player && g_engine_client->is_ingame() && g_local_player->is_alive())
		{
			if (settings::misc::viewmodel::override != 0 && !g_local_player->is_scoped())
				vsView->fov = settings::misc::viewmodel::override;

			if (settings::misc::disable_zoom && g_local_player->is_scoped())
				vsView->fov = (settings::misc::viewmodel::override == 0) ? 90 : settings::misc::viewmodel::override;
		}

		ofunc(g_client_mode, vsView);
	}
}