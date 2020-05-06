#include "aimbot.h"
#include "autowall.h"
#include "../../imgui/imgui.h"
#include "../../render/render.h"
#include "../../menu/menu.h"
#include "backtrack.h"
#include "../../helpers/math.h"
#include "../../helpers/input.h"
#include "backtrack.h"

namespace legit_aimbot
{
	int get_knife_settings()
	{
		auto pWeapon = g_local_player->active_weapon();

		if (pWeapon->is_knife())
			return 228;
		else
			return pWeapon->item().item_definition_index();
	}

	legitbot_settings* get_settings()
	{
		if (g_engine_client->is_ingame() && g_local_player)
		{
			auto pWeapon = g_local_player->active_weapon();

			if (settings::legit_bot::settings_category == 0)
				return &settings::legit_bot::legitbot_items_all;

			else if (settings::legit_bot::settings_category == 1)
			{
				if (pWeapon && pWeapon->get_cs_weapondata())
				{
					return &settings::legit_bot::legitbot_items_groups[legit_aimbot::get_group()];
				}
				else
				{
					return &settings::legit_bot::legitbot_items_groups[settings::legit_bot::weapon_group];
				}
			}
			else if (settings::legit_bot::settings_category == 2)
			{
				if (pWeapon && pWeapon->get_cs_weapondata())
				{
					return &settings::legit_bot::legitbot_items[get_knife_settings()];
				}
				else
				{
					return &settings::legit_bot::legitbot_items[menu::settings_weap];
				}
			}
		}
		else
		{
			if (settings::legit_bot::settings_category == 0)
				return &settings::legit_bot::legitbot_items_all;

			else if (settings::legit_bot::settings_category == 1)
			{
				return &settings::legit_bot::legitbot_items_groups[settings::legit_bot::weapon_group];
			}
			else if (settings::legit_bot::settings_category == 2)
			{
				return &settings::legit_bot::legitbot_items[menu::settings_weap];
			}
		}
	}

	bool is_rcs()
	{
		return g_local_player->shots_fired() >= _settings.rcs_start + 1;
	}

	float get_real_distance_fov(float distance, QAngle angle, QAngle viewangles) 
	{
		Vector aimingAt;
		math::angle_vectors(viewangles, aimingAt);
		aimingAt *= distance;
		Vector aimAt;
		math::angle_vectors(angle, aimAt);
		aimAt *= distance;
		return aimingAt.DistTo(aimAt) / 5;
	}

	float get_fov_to_player(QAngle viewAngle, QAngle aimAngle)
	{
		QAngle delta = aimAngle - viewAngle;
		math::fix_angles(delta);
		return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
	}
	
	bool is_line_goes_through_smoke(Vector vStartPos, Vector vEndPos)
	{
		static auto LineGoesThroughSmokeFn = (bool(*)(Vector vStartPos, Vector vEndPos))utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
		return LineGoesThroughSmokeFn(vStartPos, vEndPos);
	}
	int get_group()
	{
		auto pWeapon = g_local_player->active_weapon();

		if (!pWeapon || !(pWeapon->is_sniper() || pWeapon->is_pistol() || pWeapon->is_rifle() || pWeapon->is_zeus() || pWeapon->is_knife()) || pWeapon->item().item_definition_index() == WEAPON_C4)
		{
			return -1;
		}

		if (pWeapon->is_knife())
			return 0;
		else if (pWeapon->is_zeus())
			return 1;
		else if (pWeapon->is_pistol())
			return 2;
		else if (pWeapon->get_cs_weapondata()->weapon_type == WEAPONTYPE_SUBMACHINEGUN)
			return 3;
		else if (pWeapon->get_cs_weapondata()->weapon_type == WEAPONTYPE_SHOTGUN)
			return 4;
		else if (pWeapon->get_cs_weapondata()->weapon_type == WEAPONTYPE_MACHINEGUN)
			return 5;
		else if (pWeapon->get_cs_weapondata()->weapon_type == WEAPONTYPE_RIFLE)
			return 6;
		else if (pWeapon->get_cs_weapondata()->weapon_type == WEAPONTYPE_SNIPER_RIFLE)
			return 7;
	}
	//--------------------------------------------------------------------------------
	bool is_enabled(c_usercmd* pCmd)
	{
		if (!g_engine_client->is_connected() || !g_local_player || !g_local_player->is_alive()) {
			return false;
		}

		auto pWeapon = g_local_player->active_weapon();

		if (!pWeapon || !(pWeapon->is_sniper() || pWeapon->is_pistol() || pWeapon->is_rifle() || pWeapon->is_zeus() || pWeapon->is_knife()))
		{
			return false;
		}

		auto weaponData = pWeapon->get_cs_weapondata();
		auto weapontype = weaponData->weapon_type;

		if (settings::legit_bot::settings_category == 0)
			_settings = settings::legit_bot::legitbot_items_all;
		else if (settings::legit_bot::settings_category == 1)
			_settings = settings::legit_bot::legitbot_items_groups[get_group()];
		else if (settings::legit_bot::settings_category == 2)
			_settings = settings::legit_bot::legitbot_items[get_knife_settings()];

		if (!_settings.enabled)
		{
			return false;
		}

		if ((pWeapon->item().item_definition_index() == WEAPON_AWP || pWeapon->item().item_definition_index() == WEAPON_SSG08 || pWeapon->item().item_definition_index() == WEAPON_SCAR20 || pWeapon->item().item_definition_index() == WEAPON_G3SG1) && _settings.only_in_zoom && !g_local_player->is_scoped()) {
			return false;
		}

		if ((!pWeapon->is_knife() && !pWeapon->has_bullets()) || pWeapon->is_reloading()) {
			return false;
		}

		if (settings::legit_bot::bind_check && !settings::legit_bot::bind.enable)
			return false;

		return pCmd->buttons & IN_ATTACK || (pWeapon->is_knife() && pCmd->buttons & IN_ATTACK2);
	}

	float get_smooth() {
		float smooth = is_rcs() && _settings.rcs_smooth_enabled ? _settings.rcs_smooth : _settings.smooth;
		return smooth;
	}

	void smooth(QAngle currentAngle, QAngle aimAngle, QAngle& angle) {
		auto smooth_value = get_smooth();
		if (smooth_value <= 1) {
			return;
		}

		QAngle delta = aimAngle - currentAngle;
		math::fix_angles(delta);

		// ничего не трогать и не менять
		if (_settings.smooth_type == 1)
		{
			float deltaLength = fmaxf(sqrtf((delta.pitch * delta.pitch) + (delta.yaw * delta.yaw)), 0.01f);
			delta *= (1.0f / deltaLength);

			RandomSeed(g_global_vars->tickcount);
			float randomize = RandomFloat(-0.1f, 0.1f);
			smooth_value = fminf((g_global_vars->interval_per_tick * 64.0f) / (randomize + smooth_value * 0.15f), deltaLength);
		}
		else
		{
			smooth_value = (g_global_vars->interval_per_tick * 64.0f) / smooth_value;
		}

		delta *= smooth_value;
		angle = currentAngle + delta;
		math::fix_angles(angle);
	}

	void rcs(QAngle& angle, c_base_player* target, bool should_run) {
		if (!_settings.rcs) {
			rcs_last_punch.Init();
			return;
		}

		if (_settings.rcs_x == 0 && _settings.rcs_y == 0) {
			rcs_last_punch.Init();
			return;
		}

		QAngle punch = g_local_player->aim_punch_angle() * 2.0f;

		auto weapon = g_local_player	->active_weapon().get();
		if (weapon && weapon->next_primary_attack() > g_global_vars->curtime) {
			auto delta_angles = punch - rcs_last_punch;
			auto delta = weapon->next_primary_attack() - g_global_vars->curtime;
			if (delta >= g_global_vars->interval_per_tick)
				punch = rcs_last_punch + delta_angles / static_cast<float>(TIME_TO_TICKS(delta));
		}

		current_punch = punch;
		if (_settings.rcs_type == 0 && !should_run)
			punch -= { rcs_last_punch.pitch, rcs_last_punch.yaw, 0.f };

		rcs_last_punch = current_punch;
		if (!is_rcs()) {
			return;
		}

		angle.pitch -= punch.pitch * (_settings.rcs_x / 100.0f);
		angle.yaw -= punch.yaw * (_settings.rcs_y / 100.0f);

		math::fix_angles(angle);
	}

	float get_fov()
	{
		if (is_rcs() && _settings.rcs && _settings.rcs_fov_enabled)
			return _settings.rcs_fov;

		if (!silent_enabled) return _settings.fov;

		return _settings.silent_fov > _settings.fov ? _settings.silent_fov : _settings.fov;
	}

	float without_silent()
	{
		if (is_rcs() && _settings.rcs && _settings.rcs_fov_enabled)
			return _settings.rcs_fov;

		return _settings.fov;
	}

	c_base_player* get_closest_player(c_usercmd* cmd, int& bestBone) {
		QAngle ang;
		Vector eVecTarget;
		Vector pVecTarget = g_local_player->get_eye_pos();
		if (target && !kill_delay && _settings.kill_delay > 0 && target->is_not_target())
		{
			target = NULL;
			shot_delay = false;
			kill_delay = true;
			kill_delay_time = (int)GetTickCount() + _settings.kill_delay;
		}
		if (kill_delay) {
			if (kill_delay_time <= (int)GetTickCount()) kill_delay = false;
			else return NULL;
		}

		c_base_player* player;
		aim_position = Vector{ 0, 0, 0 };
		target = NULL;
		int bestHealth = 100.f;
		float bestFov = 9999.f;
		float bestDamage = 0.f;
		float bestBoneFov = 9999.f;
		float bestDistance = 9999.f;
		int health;
		float fov;
		float damage;
		float distance;

		bool hitbox_overrided = _settings.enable_hitbox_override && _settings.hitbox_override_bind.enable;

		int fromBone = 0;
		int toBone = 7;

		for (int i = 1; i < g_engine_client->get_max_clients(); ++i)
		{
			damage = 0.f;
			player = (c_base_player*)g_entity_list->get_client_entity(i);
			if (player->is_not_target()) {
				continue;
			}
			if (!_settings.deathmatch && player->team_num() == g_local_player->team_num()) {
				continue;
			}
			for (int bone = fromBone; bone <= toBone; bone++)
			{
				if (hitbox_overrided ? !_settings.override_hitboxes[bone] : !_settings.hitboxes[bone])
				{
					continue;
				}

				eVecTarget = player->get_hitbox_pos(bone);
				math::vector_angles(eVecTarget - pVecTarget, ang);
				math::fix_angles(ang);
				distance = pVecTarget.DistTo(eVecTarget);

				fov = get_fov_to_player(cmd->viewangles + rcs_last_punch, ang);

				bool backtrack_enable = backtrack::data.count(player->ent_index()) > 0 && _settings.aim_at_backtrack;

				QAngle backtrack_ang;
				Vector backtrack_eVecTarget;
				Vector backtrack_pVecTarget = g_local_player->get_eye_pos();
				float backtrack_fov = 0.f;
				float beastbacktrack_fov = 999999;
				Vector local_bb_bone_pos = Vector{ 0,0,0 };
				Vector bb_bone_pos = Vector{ 0,0,0 };

				if (backtrack_enable  && fov > get_fov())
				{
					auto& data = backtrack::data.at(player->ent_index());
					if (data.size() > 0)
					{
						Vector best_bb_bone_pos = Vector{ 0,0,0 };

						for (auto& record : data)
						{
							auto hitbox_head = record.hitboxset->get_hitbox(bone);
							auto hitbox_center = (hitbox_head->bbmin + hitbox_head->bbmax) * 0.5f;

							math::vector_transform(hitbox_center, record.boneMatrix[hitbox_head->bone], best_bb_bone_pos);

							backtrack_eVecTarget = best_bb_bone_pos;
							math::vector_angles(backtrack_eVecTarget - backtrack_pVecTarget, backtrack_ang);
							math::fix_angles(backtrack_ang);

							backtrack_fov = get_fov_to_player(cmd->viewangles + rcs_last_punch, backtrack_ang);

							if (backtrack_fov > get_fov())
								continue;

							if (beastbacktrack_fov > backtrack_fov)
							{
								beastbacktrack_fov = backtrack_fov;
								local_bb_bone_pos = best_bb_bone_pos;
							}
						}

						if (g_local_player->can_see_player(local_bb_bone_pos) > 0.9f)
						{
							bb_bone_pos = local_bb_bone_pos;
						}
					}
				}

				if (fov > get_fov() && (bb_bone_pos == Vector{ 0,0,0 } || !backtrack_enable))
					continue;

				if (!g_local_player->can_see_player(player, eVecTarget))
				{
					eVecTarget = player->get_hitbox_pos(bone);

					damage = autowall::get_damage(eVecTarget);

					if (bb_bone_pos == Vector{ 0,0,0 })
					{
						if (!_settings.autowall)
							continue;

						if (damage < _settings.min_damage)
							continue;
					}
				}

				health = player->health() - damage;
				if (_settings.smoke_check && is_line_goes_through_smoke(pVecTarget, eVecTarget))
					continue;

				bool on_ground = (g_local_player->m_flags() & FL_ONGROUND);
				if (_settings.jump_check && !on_ground)
					continue;

				bestBoneFov = fov;

				if (bb_bone_pos != Vector{ 0,0,0 })
				{
					aim_position = bb_bone_pos;
				}


				if (bestFov > fov)
				{
					bestBone = bone;
					target = player;
					bestFov = fov;
					bestHealth = health;
					bestDamage = damage;
					bestDistance = distance;
				}
			}
		}
		return target;
	}

	bool is_not_silent(float fov)
	{
		return is_rcs() || !silent_enabled || (silent_enabled && fov > _settings.silent_fov);
	}

	void on_move(c_usercmd* pCmd)
	{
		if (!is_enabled(pCmd))
		{
			if (g_local_player && g_engine_client->is_ingame() && g_local_player->is_alive() && _settings.enabled && _settings.rcs_type == 0)
			{
				auto pWeapon = g_local_player->active_weapon();
				if (pWeapon && (pWeapon->is_sniper() || pWeapon->is_pistol() || pWeapon->is_rifle()))
				{
					rcs(pCmd->viewangles, target, false);
					math::fix_angles(pCmd->viewangles);
					g_engine_client->set_view_angles(&pCmd->viewangles);
				}
			}
			else
			{
				rcs_last_punch = { 0, 0, 0 };
			}

			is_delayed = false;
			shot_delay = false;
			kill_delay = false;
			silent_enabled = _settings.silent_fov > 0;
			target = NULL;
			return;
		}

		RandomSeed(pCmd->command_number);

		auto weapon = g_local_player->active_weapon().get();
		if (!weapon)
			return;

		auto weapon_data = weapon->get_cs_weapondata();
		if (!weapon_data)
			return;

		bool should_do_rcs = false;
		QAngle angles = pCmd->viewangles;
		QAngle current = angles;
		float fov = 180.f;
		if (!(_settings.flash_check && g_local_player->is_flashed(_settings.flash_check_min)))
		{
			int bestBone = -1;
			if (get_closest_player(pCmd, bestBone))
			{
				Vector aim_pos = target->get_hitbox_pos(bestBone);

				if (aim_position != Vector{ 0,0,0 })
					aim_pos = aim_position;

				math::vector_angles(aim_pos - g_local_player->get_eye_pos(), angles);
				math::fix_angles(angles);
				fov = get_fov_to_player(pCmd->viewangles, angles);

				should_do_rcs = true;
			}
		}

		if (is_not_silent(fov) && (should_do_rcs || _settings.rcs_type == 0)) {
			rcs(angles, target, should_do_rcs);
		}

		if (target && is_not_silent(fov))
		{
			smooth(current, angles, angles);
		}
		math::fix_angles(angles);
		if (_settings.silent_chance > utils::random(0, 100))
		{
			pCmd->viewangles = angles;
		}

		if (is_not_silent(fov))
		{
			g_engine_client->set_view_angles(&angles);
		}

		silent_enabled = false;
		if (g_local_player->active_weapon()->is_pistol() && _settings.autopistol)
		{
			float server_time = g_local_player->tick_base() * g_global_vars->interval_per_tick;
			float next_shot = g_local_player->active_weapon()->next_primary_attack() - server_time;
			if (next_shot > 0) {
				pCmd->buttons &= ~IN_ATTACK;
			}
		}
	}
}