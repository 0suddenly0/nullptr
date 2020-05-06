#include "../hooks.h"

namespace hooks
{
	void __stdcall lock_cursor::hook()
	{
		static auto ofunc = vguisurf_vhook.get_original<fn>(index::lock_cursor);

		if (menu::is_visible())
		{
			g_vgui_surface->unlock_cursor();
			g_input_system->reset_input_state();
			return;
		}
		ofunc(g_vgui_surface);
	}
}