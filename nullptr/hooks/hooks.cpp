#include <intrin.h>  

#include "hooks.h"
#include "../render/render.h"
#include "../menu/menu.h"
#include "../options/options.h"
#include "../features/misc/events.h"

namespace hooks
{
	vfunc_hook netchannel_vhook;
	vfunc_hook retrieve_message_vhook;
	vfunc_hook direct3d_vhook;
	vfunc_hook vguipanel_vhook;
	vfunc_hook vguisurf_vhook;
	vfunc_hook mdlrender_vhook;
	vfunc_hook viewrender_vhook;
	vfunc_hook client_vhook;
	vfunc_hook render_view_vhook;
	vfunc_hook engine_vhook;
	vfunc_hook rend_vhook;
	vfunc_hook draw_model_overlay_vhook;
	vfunc_hook sv_cheats_vhook;
	vfunc_hook sound_vhook;
	vfunc_hook clientmode_vhook;
	vfunc_hook partition_vhook;
	vfunc_hook clientstate_vhook;
	vfunc_hook bsp_query_vhook;
	vfunc_hook gameevents_vhook;
	vfunc_hook weapon_spread_vhook;
	vfunc_hook sv_showimpacts;
	recv_prop_hook* sequence_vhook;

	void initialize()
	{
		client_vhook.setup(g_chl_client);
		render_view_vhook.setup(g_render_view);
		engine_vhook.setup(g_engine_client);
		retrieve_message_vhook.setup(g_game_coordinator);
		direct3d_vhook.setup(g_d3ddevice9);
		vguipanel_vhook.setup(g_vgui_panel);
		vguisurf_vhook.setup(g_vgui_surface);
		sound_vhook.setup(g_engine_sound);
		mdlrender_vhook.setup(g_mdl_render);
		clientmode_vhook.setup(g_client_mode);
		partition_vhook.setup(g_spatial_partition);
		clientstate_vhook.setup((void*)((uintptr_t)g_client_state + 0x8));
		bsp_query_vhook.setup(g_engine_client->get_bsp_treequery());
		gameevents_vhook.setup(g_game_events);
		convar* sv_cheats_con = g_cvar->find_var("sv_cheats");
		sv_cheats_vhook.setup(sv_cheats_con);
		convar* r_drawmodelstatsoverlay_con = g_cvar->find_var("r_drawmodelstatsoverlay");
		draw_model_overlay_vhook.setup(r_drawmodelstatsoverlay_con);
		convar* weapon_debug_spead_show_con = g_cvar->find_var("weapon_debug_spread_show");
		weapon_spread_vhook.setup(weapon_debug_spead_show_con);

		direct3d_vhook.hook_index(index::end_scene,                           end_scene::hook);
		direct3d_vhook.hook_index(index::reset,                               reset::hook);
		vguipanel_vhook.hook_index(index::paint_traverse,                     paint_treverse::hook);
		retrieve_message_vhook.hook_index(index::retrieve_message,            retrieve_message::hook);
		engine_vhook.hook_index(index::is_playing_demo,                       is_playing_demo::hook);
		clientmode_vhook.hook_index(index::create_move,                       create_move::hook);
		mdlrender_vhook.hook_index(index::draw_model_execute,                 draw_model_execute::hook);
		vguisurf_vhook.hook_index(index::lock_cursor,                         lock_cursor::hook);
		clientmode_vhook.hook_index(index::override_view,                     override_view::hook);
		client_vhook.hook_index(index::frame_stage_notify,                    frame_stage_notify::hook);
		sound_vhook.hook_index(index::emit_sound,                             emit_sound::hook);
		clientstate_vhook.hook_index(index::read_packet_entities,             read_packet_entities::hook);
		sequence_vhook = new recv_prop_hook(c_base_view_model::sequence(),    recv_proxy::hook);
		partition_vhook.hook_index(index::suppress_lists,                     suppress_lists::hook);
		bsp_query_vhook.hook_index(index::list_leaves_in_box,                 list_leaves_in_box::hook);
		sv_cheats_vhook.hook_index(index::cvar_get_bool,                      cvar_hooks::sv_cheats::hook);
		draw_model_overlay_vhook.hook_index(index::cvar_get_bool,             cvar_hooks::draw_model_overlay::hook);
		weapon_spread_vhook.hook_index(index::cvar_get_bool,                  cvar_hooks::weapon_spread::hook);
		clientmode_vhook.hook_index(index::do_post_screen_space_effects,      do_post_screen_effects::hook);

		//gameevents_vhook.hook_index(index::fire_event,                      fire_event::hook);
		/*renderview.hook_index(index::Scene_end_i,                           hkScene_end);*/

		impact::hook();
		
		events.init();
	}

	//--------------------------------------------------------------------------------
	void shutdown()
	{
		netchannel_vhook.unhook_all();
		retrieve_message_vhook.unhook_all();
		direct3d_vhook.unhook_all();
		vguipanel_vhook.unhook_all();
		vguisurf_vhook.unhook_all();
		mdlrender_vhook.unhook_all();
		viewrender_vhook.unhook_all();
		client_vhook.unhook_all();
		render_view_vhook.unhook_all();
		engine_vhook.unhook_all();
		rend_vhook.unhook_all();
		draw_model_overlay_vhook.unhook_all();
		sv_cheats_vhook.unhook_all();
		sound_vhook.unhook_all();
		clientmode_vhook.unhook_all();
		partition_vhook.unhook_all();
		clientstate_vhook.unhook_all();
		bsp_query_vhook.unhook_all();
		gameevents_vhook.unhook_all();
		sequence_vhook->~recv_prop_hook();
		sv_showimpacts.unhook_all();

		impact::destroy();

		utils::message_send();
		events.shutdown();

		/*rend_hook.unhook_all();
		r_drawmodelstatsoverlay_vhook.unhook_all();
		client_hook.unhook_all();
		netchannel_hook.unhook_all();
		engine.unhook_all();
		retrieve_message.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		gameevents_hook.unhook_all();
		sound_hook.unhook_all();
		sv_cheats.unhook_all();
		bsp_query_hook.unhook_all();
		partition_hook.unhook_all();
		clientstate_hook.unhook_all();*/
	}
}