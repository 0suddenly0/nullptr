#include "backtrack.h"
#include "aimbot.h"
#include "../../helpers/math.h"

float clamp(float& val, float& min, float& max)
{
	if (val < min)
		val = min;

	if (val > max)
		val = max;

	return val;
}

namespace backtrack
{
	std::map<int, std::deque<backtrack_data>> data = { };

	void on_move(c_usercmd* pCmd)
	{
		if (!g_engine_client->is_ingame() || !g_local_player || !g_local_player->is_alive()) {
			data.clear();
			return;
		}

		auto pWeapon = g_local_player->active_weapon();
		if (!pWeapon || !(pWeapon->is_sniper() || pWeapon->is_pistol() || pWeapon->is_rifle() || pWeapon->is_zeus() || pWeapon->is_knife())) {
			data.clear();
			return;
		}

		auto weaponData = pWeapon->get_cs_weapondata();

		/*if (settings::legit_bot::settings_category == 0)
			legit_aimbot::_settings = settings::legit_bot::legitbot_items_all;
		else if (settings::legit_bot::settings_category == 1)
			legit_aimbot::_settings = settings::legit_bot::legitbot_items_groups[legit_aimbot::get_group()];
		else if (settings::legit_bot::settings_category == 2)
			legit_aimbot::_settings = settings::legit_bot::legitbot_items[pWeapon->item().item_definition_index()];*/

		if (!legit_aimbot::_settings.enabled || legit_aimbot::_settings.backtrack_time == 0 || (settings::legit_bot::bind_check && !settings::legit_bot::bind.enable)) {
			data.clear();
			return;
		}

		static convar* sv_maxunlag = g_cvar->find_var("sv_maxunlag");
		static convar* sv_minupdaterate = g_cvar->find_var("sv_minupdaterate");
		static convar* sv_maxupdaterate = g_cvar->find_var("sv_maxupdaterate");

		static convar* sv_client_min_interp_ratio = g_cvar->find_var("sv_client_min_interp_ratio");
		static convar* sv_client_max_interp_ratio = g_cvar->find_var("sv_client_max_interp_ratio");

		static convar* cl_interp_ratio = g_cvar->find_var("cl_interp_ratio");
		static convar* cl_interp = g_cvar->find_var("cl_interp");
		static convar* cl_updaterate = g_cvar->find_var("cl_updaterate");

		float updaterate = cl_updaterate->get_float();

		float minupdaterate = sv_minupdaterate->get_float();
		float maxupdaterate = sv_maxupdaterate->get_float();

		float min_interp = sv_client_min_interp_ratio->get_float();
		float max_interp = sv_client_max_interp_ratio->get_float();

		float flLerpAmount = cl_interp->get_float();
		float flLerpRatio = cl_interp_ratio->get_float();
		float _flLerpRatio = 0;
		_flLerpRatio = std::clamp(flLerpRatio, min_interp, max_interp);
		if (_flLerpRatio == 0.0f)
			_flLerpRatio = 1.0f;

		float updateRate = 0.f;

		updateRate = std::clamp(updaterate, minupdaterate, maxupdaterate);
		lerp_time = std::fmaxf(flLerpAmount, _flLerpRatio / updateRate);
		latency = g_engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING) + g_engine_client->get_net_channel_info()->get_latency(FLOW_INCOMING);
		correct_time = latency + lerp_time;

		// setup records
		c_base_player* player;
		for (int i = 1; i <= g_global_vars->maxClients; ++i)
		{
			player = c_base_player::get_player_by_index(i);

			if (player->is_not_target()) {
				if (data.count(i) > 0)
					data.erase(i);
				continue;
			}

			if (!legit_aimbot::_settings.deathmatch && player->team_num() == g_local_player->team_num()) {
				if (data.count(i) > 0)
					data.erase(i);
				continue;
			}

			auto& cur_data = data[i];
			if (!cur_data.empty()) {
				auto& front = cur_data.front();
				if (front.simTime == player->simulation_time())
					continue;

				while (!cur_data.empty()) {
					auto& back = cur_data.back();
					float deltaTime = correct_time - (g_global_vars->curtime - back.simTime);
					if (std::fabsf(deltaTime) <= TICKS_TO_TIME(legit_aimbot::_settings.backtrack_time))
						break;

					cur_data.pop_back();
				}
			}

			auto model = player->get_model();
			if (!model)
				continue;

			auto hdr = g_mdl_info->get_studiomodel(model);
			if (!hdr)
				continue;

			auto hitbox_set = hdr->get_hitbox_set(player->hitbox_set());
			auto hitbox_head = hitbox_set->get_hitbox(HITBOX_HEAD);
			auto hitbox_center = (hitbox_head->bbmin + hitbox_head->bbmax) * 0.5f;

			backtrack_data bd;

			bd.hitboxset = hitbox_set;
			bd.player = player;
			bd.simTime = player->simulation_time();

			*(Vector*)((uintptr_t)player + 0xA0) = player->vec_origin();
			*(int*)((uintptr_t)player + 0xA68) = 0;
			*(int*)((uintptr_t)player + 0xA30) = 0;
			player->invalidate_bone_cache();
			player->setup_bones(bd.boneMatrix, 128, BONE_USED_BY_ANYTHING, g_global_vars->curtime);

			math::vector_transform(hitbox_center, bd.boneMatrix[hitbox_head->bone], bd.hitboxPos);

			float deltaTime = correct_time - (g_global_vars->curtime - bd.simTime);
			if (!(std::fabsf(deltaTime) > TICKS_TO_TIME(legit_aimbot::_settings.backtrack_time)))
			{
				data[i].push_front(bd);
			}

		}

		Vector localEyePos = g_local_player->get_eye_pos();
		QAngle angles;
		int tick_count = -1;
		float best_fov = 180.0f;
		for (auto& node : data) {
			auto& cur_data = node.second;
			if (cur_data.empty())
				continue;

			for (auto& bd : cur_data) {
				float deltaTime = correct_time - (g_global_vars->curtime - bd.simTime);
				if (std::fabsf(deltaTime) > TICKS_TO_TIME(legit_aimbot::_settings.backtrack_time))
				{
					continue;
				}

				math::vector_angles(bd.hitboxPos - localEyePos, angles);
				math::fix_angles(angles);
				float fov = legit_aimbot::get_fov_to_player(pCmd->viewangles, angles);
				if (best_fov > fov) {
					best_fov = fov;
					tick_count = TIME_TO_TICKS(bd.simTime + lerp_time);
				}
			}
		}

		if (tick_count != -1) {
			pCmd->tick_count = tick_count;
		}
	}
}

