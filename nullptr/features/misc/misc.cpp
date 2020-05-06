#include "../../valve_sdk/csgostructs.h"
#include "misc.h"
#include "../../options/options.h"
#include "../../features/notify/notify.h"
#include "../../helpers/input.h"
#include "../../helpers/math.h"


static bool enable = false;
static bool jumped = false;

enum fake_lag_types : int
{
	lag_always = 0,
	lag_only_in_air,
	lag_adaptive,
};

float angle_diff(float destAngle, float srcAngle)
{
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle) {
		if (delta >= 180)
			delta -= 360;
	}
	else {
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

#define _min(a,b) (((a) < (b)) ? (a) : (b))

namespace misc
{
	void inverse_gravity()
	{
		static auto cl_ragdoll_gravity = g_cvar->find_var("cl_ragdoll_gravity");
		static auto cl_phys_timescale = g_cvar->find_var("cl_phys_timescale");

		static const auto becup_cl_ragdoll_gravity = cl_ragdoll_gravity->get_int();
		static const auto becup_cl_phys_timescale = cl_phys_timescale->get_int();

		if (settings::misc::inverse_gravity::enable)
		{
			cl_ragdoll_gravity->m_fnChangeCallbacks.m_Size = 0;
			cl_ragdoll_gravity->set_value(settings::misc::inverse_gravity::value * 100);
		}

		if (!settings::misc::inverse_gravity::enable || g_unload)
		{
			cl_ragdoll_gravity->m_fnChangeCallbacks.m_Size = 0;
			cl_ragdoll_gravity->set_value(becup_cl_ragdoll_gravity);
		}

		if (settings::misc::inverse_gravity::enable_slow)
		{
			cl_phys_timescale->m_fnChangeCallbacks.m_Size = 0;
			cl_phys_timescale->set_value(0.2f);
		}

		if (!settings::misc::inverse_gravity::enable_slow || g_unload)
		{
			cl_phys_timescale->m_fnChangeCallbacks.m_Size = 0;
			cl_phys_timescale->set_value(becup_cl_phys_timescale);
		}
	}
	void prepare_revolver(c_usercmd* cmd)
	{
		if (!g_local_player || !g_engine_client->is_connected() || !g_local_player->active_weapon() || !g_local_player->is_alive())
			return;

		constexpr float revolverPrepareTime{ 0.234375f };

		static float readyTime;
		if (settings::misc::prepare_revolver::bind.enable)
		{
			const auto activeWeapon = g_local_player->active_weapon();
			if (activeWeapon && activeWeapon->get_item_definition_index() == ItemDefinitionIndex::WEAPON_REVOLVER)
			{
				if (!readyTime) readyTime = utils::get_curtime(cmd) + revolverPrepareTime;

				auto ticksToReady = TIME_TO_TICKS(readyTime - utils::get_curtime(cmd) - g_engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING));
				if (ticksToReady > 0 && ticksToReady <= TIME_TO_TICKS(revolverPrepareTime))
					cmd->buttons |= IN_ATTACK;
				else
					readyTime = 0.0f;
			}
		}
	}
	void moon_walk(c_usercmd* cmd)
	{
		if (settings::misc::moon_walk)
		{
			if (g_local_player->move_type() == MOVETYPE_NOCLIP)
				return;

			if (g_local_player->move_type() == MOVETYPE_LADDER)
				return;

			if (!(g_local_player->m_flags() & FL_ONGROUND))
				return;

			if (cmd->buttons & IN_FORWARD)
			{
				cmd->forwardmove = 450;
				cmd->buttons &= ~IN_FORWARD;
				cmd->buttons |= IN_BACK;
			}
			else if (cmd->buttons & IN_BACK)
			{
				cmd->forwardmove = -450;
				cmd->buttons &= ~IN_BACK;
				cmd->buttons |= IN_FORWARD;
			}

			if (cmd->buttons & IN_MOVELEFT)
			{
				cmd->sidemove = -450;
				cmd->buttons &= ~IN_MOVELEFT;
				cmd->buttons |= IN_MOVERIGHT;
			}
			else if (cmd->buttons & IN_MOVERIGHT)
			{
				cmd->sidemove = 450;
				cmd->buttons &= ~IN_MOVERIGHT;
				cmd->buttons |= IN_MOVELEFT;
			}
		}
	}
	void block_bot(c_usercmd* cmd)
	{
		if (!settings::misc::block_bot::bind.enable || !settings::misc::block_bot::enable)
			return;

		float bestdist = 200.f;
		int index = -1;

		for (int i = 1; i < 64; i++)
		{
			c_base_player* entity = (c_base_player*)g_entity_list->get_client_entity(i);

			if (!entity)
				continue;

			if (!entity->is_alive() || entity->is_dormant() || entity == g_local_player)
				continue;

			float dist = g_local_player->get_abs_origin().DistTo(entity->get_abs_origin());

			if (dist < bestdist)
			{
				bestdist = dist;
				index = i;
			}
		}

		if (index == -1)
			return;

		c_base_player* target = (c_base_player*)g_entity_list->get_client_entity(index);

		if (!target)
			return;

		{

			QAngle angles = math::calc_angle(g_local_player->get_abs_origin(), target->get_abs_origin());

			QAngle shit;

			g_engine_client->get_view_angles(&shit);

			angles.yaw -= shit.yaw;
			math::fix_angles(angles);

			if (angles.yaw < 0.20f)
				cmd->sidemove = 450.f;
			else if (angles.yaw > 0.20f)
				cmd->sidemove = -450.f;
		}
	}
	void fast_duck(c_usercmd* cmd)
	{
		if (settings::misc::fast_duck)
			cmd->buttons |= IN_BULLRUSH;
	}
	void edge_jump(c_usercmd* cmd, int old_flags)
	{
		if (!g_engine_client->is_connected())
			return;
		if (!g_local_player || !g_local_player->is_alive())
			return;

		if (settings::misc::edge_jump::enable)
		{
			if (settings::misc::edge_jump::bind.bind_type == KeyBind_type::press)
			{
				if (settings::misc::edge_jump::bind.enable)
				{
					if ((old_flags & FL_ONGROUND) && !(g_local_player->m_flags() & FL_ONGROUND))
					{
						cmd->buttons |= IN_JUMP;
						jumped = true;
					}
				}

				if (settings::misc::edge_jump::bind.enable && settings::misc::edge_jump::auto_duck && jumped)
				{
					cmd->buttons |= IN_DUCK;
				}

				if ((old_flags & FL_ONGROUND) && (g_local_player->m_flags() & FL_ONGROUND))
				{
					jumped = false;
				}
			}
			if (settings::misc::edge_jump::bind.bind_type == KeyBind_type::toggle)
			{
				if (settings::misc::edge_jump::bind.enable)
				{
					if ((old_flags & FL_ONGROUND) && !(g_local_player->m_flags() & FL_ONGROUND))
					{
						cmd->buttons |= IN_JUMP;
						jumped = true;
					}

					if (settings::misc::edge_jump::auto_duck && jumped)
					{
						cmd->buttons |= IN_DUCK;
					}

					if ((old_flags & FL_ONGROUND) && (g_local_player->m_flags() & FL_ONGROUND))
					{
						jumped = false;
					}
				}
			}
		}
	}
	void fog()
	{
		static auto fog_enable = g_cvar->find_var("fog_enable");
		static auto fog_override = g_cvar->find_var("fog_override");
		static auto fog_color = g_cvar->find_var("fog_color");
		static auto fog_start = g_cvar->find_var("fog_start");
		static auto fog_end = g_cvar->find_var("fog_end");
		static auto fog_destiny = g_cvar->find_var("fog_maxdensity");

		static auto r_modelAmbientMin = g_cvar->find_var("r_modelAmbientMin");
		static auto mat_bloomscale = g_cvar->find_var("mat_bloomscale");
		static auto mat_bloom_scalefactor_scalar = g_cvar->find_var("mat_bloom_scalefactor_scalar");

		static const auto fog_enable_bec = fog_enable->get_bool();
		static const auto fog_override_bec = fog_override->get_bool();
		static const auto fog_color_bec = fog_color->get_string();
		static const auto fog_start_bec = fog_start->get_float();
		static const auto fog_end_bec = fog_end->get_float();
		static const auto fog_destiny_bec = fog_destiny->get_float();

		if (settings::misc::fog::enable && !g_unload)
		{

			r_modelAmbientMin->set_value(settings::misc::fog::start_dist); // fog start dist
			mat_bloomscale->set_value(settings::misc::fog::end_dist); // fog end dist
			mat_bloom_scalefactor_scalar->set_value(settings::misc::flash_alpha); //max alpha fog


			/*fog_enable->m_fnChangeCallbacks.m_Size = 0;
			fog_override->m_fnChangeCallbacks.m_Size = 0;
			fog_color->m_fnChangeCallbacks.m_Size = 0;
			fog_start->m_fnChangeCallbacks.m_Size = 0;
			fog_end->m_fnChangeCallbacks.m_Size = 0;
			fog_destiny->m_fnChangeCallbacks.m_Size = 0;

			fog_enable->set_value(settings::misc::fog::enable); //fog enable
			fog_override->set_value(settings::misc::fog::enable); //fog bypass
			fog_color->set_value(std::string(std::to_string(settings::misc::fog::color.r()) + " " + std::to_string(settings::misc::fog::color.g()) + " " + std::to_string(settings::misc::fog::color.b())).c_str()); //fog color
			fog_start->set_value(settings::misc::fog::start_dist); // fog start dist
			fog_end->set_value(settings::misc::fog::end_dist); // fog end dist
			fog_destiny->set_value(settings::misc::fog::color.save_color[3]); //max alpha fog*/
		}
		else if (g_unload || !settings::misc::fog::enable)
		{
			fog_enable->m_fnChangeCallbacks.m_Size = 0;
			fog_override->m_fnChangeCallbacks.m_Size = 0;
			fog_color->m_fnChangeCallbacks.m_Size = 0;
			fog_start->m_fnChangeCallbacks.m_Size = 0;
			fog_end->m_fnChangeCallbacks.m_Size = 0;
			fog_destiny->m_fnChangeCallbacks.m_Size = 0;

			fog_enable->set_value(fog_enable_bec); //fog enable
			fog_override->set_value(fog_override_bec); //fog bypass
			fog_color->set_value(fog_color_bec); //fog color
			fog_start->set_value(fog_start_bec); // fog start dist
			fog_end->set_value(fog_end_bec); // fog end dist
			fog_destiny->set_value(fog_destiny_bec); //max alpha fog
		}
	}
	void no_smoke()
	{
		static DWORD smoke_count;
		static uint8_t* offset;

		if (!offset)
			offset = utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");

		if (!smoke_count)
			smoke_count = *reinterpret_cast<DWORD*>(offset + 0x8);

		if (settings::misc::disable_smoke)
			*reinterpret_cast<int*>(smoke_count) = 0;

		static bool set = true;
		static std::vector<const char*> smoke_materials =
		{
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
		};

		if (!settings::misc::disable_smoke || g_unload)
		{
			if (set)
			{
				for (auto material_name : smoke_materials)
				{
					i_material* mat = g_mat_system->find_material(material_name, TEXTURE_GROUP_OTHER);
					mat->set_material_var_flag(MATERIAL_VAR_WIREFRAME, false);
				}
				set = false;
			}
			return;
		}

		set = true;
		for (auto material_name : smoke_materials)
		{
			i_material* mat = g_mat_system->find_material(material_name, TEXTURE_GROUP_OTHER);
			mat->set_material_var_flag(MATERIAL_VAR_WIREFRAME, true);
		}
	}

	void disable_flash_alpha()
	{
		if (settings::misc::disable_flash)
			g_local_player->flash_max_alpha() = settings::misc::flash_alpha;
		else
			g_local_player->flash_max_alpha() = 255.f;
	}

	namespace clan_tag
	{
		int user_index;

		size_t pos = 0;
		std::string clantag;
		float last_time = 0;

		void init()
		{
			if (!settings::misc::clantag::enable || !g_local_player)
				return;

			int tick = int(g_global_vars->curtime * 2.4f);

			static std::string local_tag;

			if (settings::misc::clantag::clantag_type == 0)
			{
				switch (settings::misc::clantag::animation_type)
				{
				case 0:
					switch (tick % 26)
					{
					case 0:	 utils::set_clantag(u8"|nullptr|"); break;
					case 1:	 utils::set_clantag(u8"|nullpt| "); break;
					case 2:  utils::set_clantag(u8"|nullp|  "); break;
					case 3:	 utils::set_clantag(u8"|null|   "); break;
					case 5:	 utils::set_clantag(u8"|nul|    "); break;
					case 6:  utils::set_clantag(u8"|nu|     "); break;
					case 7:  utils::set_clantag(u8"|n|      "); break;
					case 8:	 utils::set_clantag(u8"||       "); break;
					case 9:  utils::set_clantag(u8" ||      "); break;
					case 10: utils::set_clantag(u8"  ||     "); break;
					case 11: utils::set_clantag(u8"   ||    "); break;
					case 12: utils::set_clantag(u8"    ||   "); break;
					case 13: utils::set_clantag(u8"     ||  "); break;
					case 14: utils::set_clantag(u8"      || "); break;
					case 15: utils::set_clantag(u8"       ||"); break;
					case 16: utils::set_clantag(u8"      |r|"); break;
					case 17: utils::set_clantag(u8"     |tr|"); break;
					case 18: utils::set_clantag(u8"    |ptr|"); break;
					case 19: utils::set_clantag(u8"   |lptr|"); break;
					case 20: utils::set_clantag(u8"  |llptr|"); break;
					case 21: utils::set_clantag(u8" |ullptr|"); break;
					case 22: utils::set_clantag(u8"|nullptr|"); break;
					case 23: utils::set_clantag(u8"|nullptr|"); break;
					case 24: utils::set_clantag(u8"|nullptr|"); break;
					case 25: utils::set_clantag(u8"|nullptr|"); break;
					}

					break;
				case 1:
					switch (tick % 18)
					{
					case 0:  utils::set_clantag("         "); break;
					case 1:  utils::set_clantag("        g"); break;
					case 2:  utils::set_clantag("       ga"); break;
					case 3:  utils::set_clantag("      gam"); break;
					case 4:  utils::set_clantag("     game"); break;
					case 5:  utils::set_clantag("    games"); break;
					case 6:  utils::set_clantag("   gamese"); break;
					case 7:  utils::set_clantag("  gamesen"); break;
					case 8:  utils::set_clantag(" gamesens"); break;
					case 9:  utils::set_clantag("gamesense"); break;
					case 10: utils::set_clantag("gamesense"); break;
					case 11: utils::set_clantag("amesense "); break;
					case 12: utils::set_clantag("mesense  "); break;
					case 13: utils::set_clantag("esense   "); break;
					case 14: utils::set_clantag("sense    "); break;
					case 15: utils::set_clantag("ense     "); break;
					case 16: utils::set_clantag("nse      "); break;
					case 17: utils::set_clantag("se       "); break;
					case 18: utils::set_clantag("e        "); break;
					}
					break;
				case 2:
					switch (tick % 13)
					{
					case 0:  utils::set_clantag("AIMWARE.net"); break;
					case 1:  utils::set_clantag("IMWARE.net "); break;
					case 2:  utils::set_clantag("MWARE.net A"); break;
					case 3:  utils::set_clantag("WARE.net AI"); break;
					case 4:  utils::set_clantag("ARE.net AIM"); break;
					case 5:  utils::set_clantag("RE.net AIMW"); break;
					case 6:  utils::set_clantag("E.net AIMWA"); break;
					case 7:  utils::set_clantag(".net AIMWAR"); break;
					case 8:  utils::set_clantag("net AIMWARE"); break;
					case 9:  utils::set_clantag("et AIMWARE."); break;
					case 10: utils::set_clantag("t AIMWARE.n"); break;
					case 11: utils::set_clantag(" AIMWARE.ne"); break;
					case 12: utils::set_clantag("AIMWARE.net"); break;
					case 13: utils::set_clantag("AIMWARE.net"); break;
					}
					break;
				case 3:
					switch (tick % 15)
					{
					case 0:   utils::set_clantag("clown");     break;
					case 1:   utils::set_clantag("c lown");    break;
					case 2:   utils::set_clantag("c low n");   break;
					case 3:   utils::set_clantag("c l ow n");  break;
					case 4:   utils::set_clantag("c l o w n"); break;
					case 5:   utils::set_clantag("c l o w n"); break;
					case 6:   utils::set_clantag(" c l o w "); break;
					case 7:   utils::set_clantag("  c l o w"); break;
					case 8:   utils::set_clantag("   c l o "); break;
					case 9:   utils::set_clantag("    c l o"); break;
					case 10:  utils::set_clantag("     c l "); break;
					case 11:  utils::set_clantag("      c l"); break;
					case 12:  utils::set_clantag("       c "); break;
					case 13:  utils::set_clantag("        c"); break;
					case 14:  utils::set_clantag("         "); break;
					case 15:  utils::set_clantag("n        "); break;
					case 16:  utils::set_clantag("w n      "); break;
					case 17:  utils::set_clantag("o w n    "); break;
					case 18:  utils::set_clantag("l o w n  "); break;
					case 19:  utils::set_clantag("c l o w n"); break;
					case 20:  utils::set_clantag("c l o w n"); break;
					case 21:  utils::set_clantag("c l ow n");  break;
					case 22:  utils::set_clantag("c low n");   break;
					case 23:  utils::set_clantag("c lown");    break;
					}
					break;
				}
			}
			else if (settings::misc::clantag::clantag_type == 1)
			{
				if (user_index == 0)
					return;

				auto* player = c_base_player::get_player_by_index(user_index);

				if (!player)
					return;

				const auto info = player->get_player_info();
				if (info.fakeplayer)
					return;

				auto usertag = std::string(g_player_resource->clantag()[player->get_index()]);

				static float LastChangeTime = 0.f;

				static bool restore = false;

				if (true)
				{
					if (g_global_vars->realtime - LastChangeTime >= 0.0001)
					{
						LastChangeTime = g_global_vars->realtime;

						utils::set_clantag(usertag.c_str());

						restore = true;
					}
				}
				else if (restore)
				{
					restore = false;
					utils::set_clantag("");
				}
			}
			else if (settings::misc::clantag::clantag_type == 2)
			{
				if (settings::misc::clantag::clantag.empty() || settings::misc::clantag::clantag.length() == 0)
					return;

				if (settings::misc::clantag::custom_type == 0)
				{
					switch (tick % 2)
					{
					case 1: utils::set_clantag(settings::misc::clantag::clantag, false); break;
					}
				}
				else if (settings::misc::clantag::custom_type == 1)
				{
					if (clantag != settings::misc::clantag::clantag || clantag.length() < pos)
					{
						clantag = settings::misc::clantag::clantag;
						pos = 0;
					}

					if (last_time + settings::misc::clantag::speed > g_global_vars->realtime)
						return;

					last_time = g_global_vars->realtime;

					utils::set_clantag(clantag.substr(0, pos).c_str());
					pos++;
				}
				else if (settings::misc::clantag::custom_type == 2)
				{

					static float LastChangeTime = 0.f;

					static bool restore = false;

					if (true)
					{
						if (g_global_vars->realtime - LastChangeTime >= settings::misc::clantag::speed)
						{
							settings::misc::clantag::clantag_visible;

							LastChangeTime = g_global_vars->realtime;

							std::string temp = settings::misc::clantag::clantag_visible;
							settings::misc::clantag::clantag_visible.erase(0, 1);
							settings::misc::clantag::clantag_visible += temp[0];

							utils::set_clantag(settings::misc::clantag::clantag_visible.data());

							restore = true;
						}
					}
					else if (restore)
					{
						restore = false;
						utils::set_clantag("");
					}
				}
			}
		}
	}
	namespace fake_latency
	{
		void update_sequence()
		{
			if (!g_client_state)
				return;

			auto net_channel = g_client_state->net_channel;

			if (net_channel) {
				if (net_channel->in_sequence_nr > m_last_incoming_sequence_number) {
					m_last_incoming_sequence_number = net_channel->in_sequence_nr;
					sequences.push_front(incoming_sequence_t(net_channel->in_reliable_state, net_channel->out_reliable_state, net_channel->in_sequence_nr, g_global_vars->realtime));
				}

				if (sequences.size() > 2048)
					sequences.pop_back();
			}
		}

		void clear_sequence()
		{
			m_last_incoming_sequence_number = 0;

			sequences.clear();
		}

		void add_latency(i_net_channel* net_channel)
		{
			for (auto& seq : sequences) {
				if (g_global_vars->realtime - seq.m_current_time >= settings::misc::fake_latency::amount / 1000.f) {
					net_channel->in_reliable_state = seq.m_in_reliable_state;
					net_channel->in_sequence_nr = seq.m_sequence_nr;

					break;
				}
			}
		}
	}
	namespace bhop
	{
		void on_create_move(c_usercmd* cmd)
		{
			if (!settings::misc::bhop::enable)
				return;

			static bool jumped_last_tick = false;
			static bool should_fake_jump = false;

			if (!jumped_last_tick && should_fake_jump)
			{
				should_fake_jump = false;
				cmd->buttons |= IN_JUMP;
			}
			else if (cmd->buttons & IN_JUMP)
			{
				if (g_local_player->m_flags() & FL_ONGROUND)
				{
					jumped_last_tick = true;
					should_fake_jump = true;
				}
				else
				{
					cmd->buttons &= ~IN_JUMP;
					jumped_last_tick = false;
				}
			}
			else
			{
				jumped_last_tick = false;
				should_fake_jump = false;
			}
		}
		void auto_strafe(c_usercmd* cmd, QAngle va)
		{
			if (!settings::misc::bhop::auto_strafer)
				return;

			if (settings::misc::bhop::strafer_type == 0)
			{
				if (!g_local_player || !g_local_player->is_alive() || g_local_player->move_type() != MOVETYPE_WALK)
					return;

				bool on_ground = (g_local_player->m_flags() & FL_ONGROUND) && !(cmd->buttons & IN_JUMP);
				if (on_ground) {
					return;
				}

				static auto side = 1.0f;
				side = -side;

				auto velocity = g_local_player->velocity();
				velocity.z = 0.0f;

				QAngle wish_angle = cmd->viewangles;

				auto speed = velocity.Length2D();
				auto ideal_strafe = std::clamp(RAD2DEG(atan(15.f / speed)), 0.0f, 90.0f);

				if (cmd->forwardmove > 0.0f)
					cmd->forwardmove = 0.0f;

				static auto cl_sidespeed = g_cvar->find_var("cl_sidespeed");

				static float old_yaw = 0.f;
				auto yaw_delta = std::remainderf(wish_angle.yaw - old_yaw, 360.0f);
				auto abs_angle_delta = abs(yaw_delta);
				old_yaw = wish_angle.yaw;

				if (abs_angle_delta <= ideal_strafe || abs_angle_delta >= 30.0f) {
					QAngle velocity_direction;
					math::vector_angles(velocity, velocity_direction);
					auto velocity_delta = std::remainderf(wish_angle.yaw - velocity_direction.yaw, 360.0f);
					auto retrack = std::clamp(RAD2DEG(atan(30.0f / speed)), 0.0f, 90.0f) * settings::misc::bhop::retrack_speed;
					if (velocity_delta <= retrack || speed <= 15.0f) {
						if (-(retrack) <= velocity_delta || speed <= 15.0f) {
							wish_angle.yaw += side * ideal_strafe;
							cmd->sidemove = cl_sidespeed->get_float() * side;
						}
						else {
							wish_angle.yaw = velocity_direction.yaw - retrack;
							cmd->sidemove = cl_sidespeed->get_float();
						}
					}
					else {
						wish_angle.yaw = velocity_direction.yaw + retrack;
						cmd->sidemove = -cl_sidespeed->get_float();
					}

					math::movement_fix(cmd, wish_angle, cmd->viewangles);
				}
				else if (yaw_delta > 0.0f) {
					cmd->sidemove = -cl_sidespeed->get_float();
				}
				else if (yaw_delta < 0.0f) {
					cmd->sidemove = cl_sidespeed->get_float();
				}

#if 0
				static bool leftRight;

				bool inMove = cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT;

				if (cmd->buttons & IN_FORWARD && g_local_player->velocity().Length() <= 50.0f)
					cmd->forwardmove = 250.0f;

				float yaw_change = 0.0f;

				if (g_local_player->velocity().Length() > 50.f)
					yaw_change = 30.0f * fabsf(30.0f / g_local_player->velocity().Length());

				C_BaseCombatWeapon* ActiveWeapon = g_local_player->active_weapon();

				if (ActiveWeapon && ActiveWeapon->CanFire() && cmd->buttons & IN_ATTACK)
					yaw_change = 0.0f;

				QAngle viewAngles = va;

				bool OnGround = (g_local_player->flags() & FL_ONGROUND);
				if (!OnGround && !inMove) {
					if (leftRight || cmd->mousedx > 1) {
						viewAngles.yaw += yaw_change;
						cmd->sidemove = 350.0f;
					}
					else if (!leftRight || cmd->mousedx < 1) {
						viewAngles.yaw -= yaw_change;
						cmd->sidemove = -350.0f;
					}

					leftRight = !leftRight;
				}
				viewAngles.Normalize();
				Math::ClampAngles(viewAngles);
#endif
			}
			else if (settings::misc::bhop::strafer_type == 1)
			{
				static bool is_bhopping;
				static float calculated_direction;
				static bool in_transition;
				static float true_direction;
				static float wish_direction;
				static float step;
				static float rough_direction;

				enum directions
				{
					FORWARDS = 0,
					BACKWARDS = 180,
					LEFT = 90,
					RIGHT = -90
				};

				// Reset direction when player is not strafing
				is_bhopping = cmd->buttons & IN_JUMP;
				if (!is_bhopping && g_local_player->m_flags() & FL_ONGROUND)
				{
					calculated_direction = directions::FORWARDS;
					in_transition = false;
					return;
				}

				// Get true view angles
				QAngle base{ };
				g_engine_client->get_view_angles(&base);

				// Calculate the rough direction closest to the player's true direction
				auto get_rough_direction = [&](float true_direction) -> float
				{
					// Make array with our four rough directions
					std::array< float, 4 > minimum = { directions::FORWARDS, directions::BACKWARDS, directions::LEFT, directions::RIGHT };
					float best_angle, best_delta = 181.f;

					// Loop through our rough directions and find which one is closest to our true direction
					for (size_t i = 0; i < minimum.size(); ++i)
					{
						float rough_direction = base.yaw + minimum.at(i);
						float delta = fabsf(math::normalize_yaw(true_direction - rough_direction));

						// Only the smallest delta wins out
						if (delta < best_delta)
						{
							best_angle = rough_direction;
							best_delta = delta;
						}
					}

					return best_angle;
				};

				// Get true direction based on player velocity
				true_direction = g_local_player->velocity().Angle().y;

				// Detect wish direction based on movement keypresses
				if (cmd->buttons & IN_FORWARD)
				{
					wish_direction = base.yaw + directions::FORWARDS;
				}
				else if (cmd->buttons & IN_BACK)
				{
					wish_direction = base.yaw + directions::BACKWARDS;
				}
				else if (cmd->buttons & IN_MOVELEFT)
				{
					wish_direction = base.yaw + directions::LEFT;
				}
				else if (cmd->buttons & IN_MOVERIGHT)
				{
					wish_direction = base.yaw + directions::RIGHT;
				}
				else
				{
					// Reset to forward when not pressing a movement key, then fix anti-aim strafing by setting IN_FORWARD
					cmd->buttons |= IN_FORWARD;
					wish_direction = base.yaw + directions::FORWARDS;
				}
				// Calculate the ideal strafe rotation based on player speed (c) navewindre
				float speed_rotation = std::min(RAD2DEG(std::asinf(30.f / g_local_player->velocity().Length2D())) * 0.5f, 45.f);
				if (in_transition)
				{
					// Get value to rotate by via calculated speed rotation
					float ideal_step = speed_rotation + calculated_direction;
					step = fabsf(math::normalize_yaw(calculated_direction - ideal_step)); // 15.f is a good alternative, but here is where you would do your "speed" slider value for the autostrafer

					// Check when the calculated direction arrives close to the wish direction
					if (fabsf(math::normalize_yaw(wish_direction - calculated_direction)) > step)
					{
						float add = math::normalize_yaw(calculated_direction + step);
						float sub = math::normalize_yaw(calculated_direction - step);

						// Step in direction that gets us closer to our wish direction
						if (fabsf(math::normalize_yaw(wish_direction - add)) >= fabsf(math::normalize_yaw(wish_direction - sub)))
						{
							calculated_direction -= step;
						}
						else
						{
							calculated_direction += step;
						}
					}
					else
					{
						// Stop transitioning when we meet our wish direction
						in_transition = false;
					}
				}
				else
				{
					// Get rough direction and setup calculated direction only when not transitioning
					rough_direction = get_rough_direction(true_direction);
					calculated_direction = rough_direction;

					// When we have a difference between our current (rough) direction and our wish direction, then transition
					if (rough_direction != wish_direction)
					{
						in_transition = true;
					}
				}

				// Set movement up to be rotated
				cmd->forwardmove = 0.f;
				cmd->sidemove = cmd->command_number % 2 ? 450.f : -450.f;

				// Calculate ideal rotation based on our newly calculated direction
				float direction = (cmd->command_number % 2 ? speed_rotation : -speed_rotation) + calculated_direction;

				// Rotate our direction based on our new, defininite direction
				float rotation = DEG2RAD(base.yaw - direction);

				float cos_rot = cos(rotation);
				float sin_rot = sin(rotation);

				float forwardmove = (cos_rot * cmd->forwardmove) - (sin_rot * cmd->sidemove);
				float sidemove = (sin_rot * cmd->forwardmove) + (cos_rot * cmd->sidemove);

				// Apply newly rotated movement
				cmd->forwardmove = forwardmove;
				cmd->sidemove = sidemove;
			}
		}
	}
	namespace desync
	{
		void next_lby_update(c_usercmd* cmd, bool* sendpacket)
		{

		}

		bool is_enabled(c_usercmd* cmd)
		{
			if (!g_local_player || !g_local_player->is_alive())
				return false;

			if (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2)
				return false;

			if (!settings::misc::desync::type || (cmd->buttons & IN_USE))
				return false;

			auto* channel_info = g_engine_client->get_net_channel_info();
			if (channel_info && (channel_info->get_avg_loss(1) > 0.f || channel_info->get_avg_loss(0) > 0.f))
				return false;

			if (g_local_player->gun_game_immunity() || g_local_player->m_flags() & FL_FROZEN)
				return false;

			if (g_local_player->move_type() == MOVETYPE_LADDER || g_local_player->move_type() == MOVETYPE_NOCLIP)
				return false;

			return true;
		}

		bool is_firing(c_usercmd* cmd)
		{
			auto weapon = g_local_player->active_weapon();
			if (!weapon)
				return false;

			const auto weapon_type = weapon->get_cs_weapondata()->weapon_type;
			if (weapon_type == WEAPONTYPE_GRENADE)
			{
				return true;

				//if (!weapon->m_bPinPulled() && (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2))
					//return false;
			}
			else if (weapon_type == WEAPONTYPE_KNIFE)
			{
				if (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2)
					return true;
			}
			else if (weapon->get_item_definition_index() == WEAPON_REVOLVER)
			{
				if (cmd->buttons & IN_ATTACK && weapon->can_fire())
					return true;

				if (cmd->buttons & IN_ATTACK2)
					return true;
			}
			else if (cmd->buttons & IN_ATTACK && weapon->can_fire() && weapon_type != WEAPONTYPE_C4)
				return true;

			return false;
		}

		void handle(c_usercmd* cmd, QAngle currentViewAngles, bool* send_packet)
		{
			static bool broke = false;
			side = settings::misc::desync::side;

			if (!is_enabled(cmd))
			{
				return;
			}

			float curtime = utils::get_curtime(cmd);

			auto am = g_local_player->get_player_anim_state();

			if (settings::misc::desync::type == 2)
			{
				/*if (!*send_packet)
				{
					if (m_bBreakLowerBody)
					{
						Logger::Screen::AddLog("debug", "1");
						cmd->viewangles.yaw += 120 * side;
					}
					else
					{
						cmd->viewangles.yaw += 120 * -side;
					}
				}*/
				/*if (*send_packet)
				{
					cmd->viewangles.yaw += g_local_player->get_max_desync_delta();
				}
				else*/
				if (!*send_packet)
				{
					if (m_bBreakLowerBody)
					{
						cmd->viewangles.yaw -= 122.0f;
					}
					//if(!m_bBreakLowerBody)
					//{
					cmd->viewangles.yaw += 120.0f /*desync_delta() * 2*/;
					//}
				}
				/*else
				{
					cmd->viewangles.yaw += g_local_player->get_max_desync_delta();
				}*/

				/*if (m_bBreakLowerBody)
				{
					cmd->viewangles.yaw += 120 * side;
				}*/

			}
			else if (settings::misc::desync::type == 1)
			{
				if (fabsf(cmd->sidemove) < 5.0f) {
					if (cmd->buttons & IN_DUCK)
						cmd->sidemove = cmd->tick_count & 1 ? 3.25f : -3.25f;
					else
						cmd->sidemove = cmd->tick_count & 1 ? 1.1f : -1.1f;
				}
			}

			math::fix_angles(cmd->viewangles);
			math::movement_fix(cmd, currentViewAngles, cmd->viewangles);
		}
	}
	namespace fake_lags
	{
		static int max_choke_ticks = 14;
		void handle(c_usercmd* cmd, bool* send_packet)
		{
			static int latency_ticks = 0;
			float fl_latency = g_engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING);
			int latency = TIME_TO_TICKS(fl_latency);
			if (g_client_state->choked_commands <= 0) {
				latency_ticks = latency;
			}
			else {
				latency_ticks = std::max(latency, latency_ticks);
			}

			if (g_gamerules->is_valve_ds())
			{
				if (fl_latency >= g_global_vars->interval_per_tick)
					max_choke_ticks = 11 - latency_ticks;
				else
					max_choke_ticks = 11;
			}
			else {
				max_choke_ticks = 13 - latency_ticks;
			}

			/*if (g_Options.fakelag_unducking &&
				g_local_player->m_flDuckAmount() > 0.0f && g_local_player->m_flDuckAmount() < 1.0f) {
				*send_packet = !(g_client_state->chokedcommands < max_choke_ticks);
				return;
			}*/

			if (settings::misc::fake_lag::factor <= 0)
				return;

			int choke_factor = settings::misc::desync::type ? std::min(max_choke_ticks, settings::misc::fake_lag::factor) : settings::misc::fake_lag::factor;

			auto LegitPeek = [choke_factor](c_usercmd * cmd, bool* send_packet) {
				static bool m_bIsPeeking = false;
				if (m_bIsPeeking) {
					*send_packet = !(g_client_state->choked_commands < choke_factor);
					if (*send_packet)
						m_bIsPeeking = false;
					return;
				}

				auto speed = g_local_player->velocity().Length();
				if (speed <= 100.0f)
					return;

				auto collidable = g_local_player->get_collideable();

				Vector min, max;
				min = collidable->obb_mins();
				max = collidable->obb_maxs();

				min += g_local_player->vec_origin();
				max += g_local_player->vec_origin();

				Vector center = (min + max) * 0.5f;

				for (int i = 1; i <= g_global_vars->maxClients; ++i)
				{
					auto player = c_base_player::get_player_by_index(i);
					if (!player || !player->is_alive() || player->is_dormant())
						continue;
					if (player == g_local_player || g_local_player->team_num() == player->team_num())
						continue;

					auto weapon = player->active_weapon().get();
					if (!weapon || weapon->clip1() <= 0)
						continue;

					auto weapon_data = weapon->get_cs_weapondata();
					if (!weapon_data || weapon_data->weapon_type <= WEAPONTYPE_KNIFE || weapon_data->weapon_type >= WEAPONTYPE_C4)
						continue;

					auto eye_pos = player->get_eye_pos();

					Vector direction;
					math::angle_vectors(player->eye_angles(), direction);
					direction.NormalizeInPlace();

					Vector hit_point;
					bool hit = math::intersection_bounding_box(eye_pos, direction, min, max, &hit_point);
					if (hit && eye_pos.DistTo(hit_point) <= weapon_data->range) {
						ray_t ray;
						trace_t tr;
						c_tracefilter_skipentity filter(player);
						ray.init(eye_pos, hit_point);

						g_engine_trace->trace_ray(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &tr);
						if (tr.contents & CONTENTS_WINDOW) { // skip windows
																									// at this moment, we dont care about local player
							filter.pSkip = tr.hit_entity;
							ray.init(tr.endpos, hit_point);
							g_engine_trace->trace_ray(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &tr);
						}

						if (tr.fraction == 1.0f || tr.hit_entity == g_local_player) {
							m_bIsPeeking = true;
							break;
						}
					}
				}
			};

			auto speed = g_local_player->velocity().Length();
			bool standing = speed <= 1.0f;
			if (!settings::misc::fake_lag::lag_when_stand && standing) {
				return;
			}

			if (!settings::misc::fake_lag::lag_when_move && !standing) {
				return;
			}

			enum FakelagMode {
				FakelagStatic = 0,
				FakelagSwitch,
				FakelagAdaptive,
				FakelagRandom,
				FakelagLegitPeek
			};

			float UnitsPerTick = 0.0f;

			int WishTicks = 0;
			int AdaptiveTicks = 2;
			static int LastRandomNumber = 5;
			static int randomSeed = 12345;

			switch (settings::misc::fake_lag::type) {
			case FakelagSwitch:
				// apply same logic as static fakelag
				if (cmd->command_number % 30 > 15)
					break;
			case FakelagStatic:
				*send_packet = !(g_client_state->choked_commands < choke_factor);
				break;
			case FakelagAdaptive:
				if (standing) {
					*send_packet = !(g_client_state->choked_commands < choke_factor);
					break;
				}

				UnitsPerTick = g_local_player->velocity().Length() * g_global_vars->interval_per_tick;
				while ((WishTicks * UnitsPerTick) <= 68.0f) {
					if (((AdaptiveTicks - 1) * UnitsPerTick) > 68.0f) {
						++WishTicks;
						break;
					}
					if ((AdaptiveTicks * UnitsPerTick) > 68.0f) {
						WishTicks += 2;
						break;
					}
					if (((AdaptiveTicks + 1) * UnitsPerTick) > 68.0f) {
						WishTicks += 3;
						break;
					}
					if (((AdaptiveTicks + 2) * UnitsPerTick) > 68.0f) {
						WishTicks += 4;
						break;
					}
					AdaptiveTicks += 5;
					WishTicks += 5;
					if (AdaptiveTicks > 16)
						break;
				}

				*send_packet = !(g_client_state->choked_commands < WishTicks);
				break;
			case FakelagRandom:
				if (g_client_state->choked_commands < LastRandomNumber) {
					*send_packet = false;
				}
				else {
					randomSeed = 0x41C64E6D * randomSeed + 12345;
					LastRandomNumber = (randomSeed / 0x10000 & 0x7FFFu) % choke_factor;
					*send_packet = true;
				}
				break;
			case FakelagLegitPeek:
				LegitPeek(cmd, send_packet);
				break;
			}
		}
	}
}