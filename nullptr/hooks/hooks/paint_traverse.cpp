#include "../hooks.h"

namespace hooks
{
	void __stdcall paint_treverse::hook(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_vhook.get_original<fn>(index::paint_traverse);


		if (settings::misc::disable_zoom_border && !strcmp("HudZoom", g_vgui_panel->get_name(panel)) && g_local_player->is_alive())
			return;

		oPaintTraverse(g_vgui_panel, panel, forceRepaint, allowForce);

		if (!panelId)
		{
			const auto panelName = g_vgui_panel->get_name(panel);
			if (!strcmp(panelName, "MatSystemTopPanel"))
			{
				panelId = panel;
			}
		}
		else if (panelId == panel)
		{

			if (g_local_player && IN_SCORE && settings::misc::reveal_rank)
				utils::rank_reveal_all();

			render::begin_scene();
		}
	}
}