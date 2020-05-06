#include "../hooks.h"

namespace hooks
{
	bool __stdcall create_move::hook(float input_sample_frametime, c_usercmd* cmd)
	{
		bool* send_packet = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 0x14);
		auto oCreateMove = clientmode_vhook.get_original<fn>(index::create_move);
		bool result = oCreateMove(g_client_mode, input_sample_frametime, cmd);

		if (!cmd || !cmd->command_number || g_unload)
			return result;

		static int latency_ticks = 0;
		float fl_latency = g_engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING);
		int latency = TIME_TO_TICKS(fl_latency);
		int old_flags = g_local_player->m_flags();
		QAngle OldAngles = cmd->viewangles;

		if (g_client_state->choked_commands <= 0)
		{
			latency_ticks = latency;
		}
		else
		{
			latency_ticks = max(latency, latency_ticks);
		}

		if (menu::is_visible())
			cmd->buttons &= ~(IN_ATTACK | IN_ATTACK2);

		misc::fast_duck(cmd);
		misc::moon_walk(cmd);
		misc::prepare_revolver(cmd);
		misc::bhop::on_create_move(cmd);
		misc::bhop::auto_strafe(cmd, cmd->viewangles);
		misc::disable_flash_alpha();
		misc::clan_tag::init();

		if (settings::misc::fake_latency::enable && !netchannel_vhook.class_base)
		{
			auto netchannel = g_client_state->net_channel;
			if (netchannel)
			{
				netchannel_vhook.setup(netchannel);
				netchannel_vhook.hook_index(index::send_datagram, send_datagram::hook);
			}
		}

		engine_prediction::type2::start(cmd);
		{
			misc::desync::next_lby_update(cmd, send_packet);

			if (settings::misc::desync::type && std::fabsf(g_local_player->spawn_time() - g_global_vars->curtime) > 1.0f)
				misc::desync::handle(cmd, OldAngles, send_packet);


			if (*send_packet)
			{
				//real_angle = g_AnimState.m_flGoalFeetYaw;
				//if (anim_state)
			  //	  fake_angle = anim_state->m_flGoalFeetYaw;
				globals::aa::view_angle = cmd->viewangles.yaw;
			}

			/*
					if (cmd->command_number % 2 == 1 && *send_packet && settings::misc::desync::type)
				*send_packet = false;

			if (settings::misc::desync::type && g_client_state->chokedcommands >= max_choke_ticks) {
				*send_packet = true;
				cmd->viewangles = g_client_state->viewangles;
			}
			*/

			math::fix_angles(cmd->viewangles);
			cmd->viewangles.yaw = std::remainderf(cmd->viewangles.yaw, 360.0f);
		}
		engine_prediction::type2::finish(cmd);

		engine_prediction::type1::begin(cmd);
		{
			misc::fake_lags::handle(cmd, send_packet);
			misc::block_bot(cmd);
			misc::edge_jump(cmd, old_flags);

			legit_aimbot::on_move(cmd);

			backtrack::on_move(cmd);

			grenade_prediction::tick(cmd->buttons);

			math::fix_angles(cmd->viewangles);
		}
		engine_prediction::type1::end();

		return false;
	}
}