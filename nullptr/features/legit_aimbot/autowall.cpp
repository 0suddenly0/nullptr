#include "autowall.h"
#include "../../valve_sdk/interfaces/i_engine_trace.h"
#include "../../helpers/math.h"
#include "../../helpers/utils.h"

#define HITGROUP_GENERIC 0
#define HITGROUP_HEAD 1
#define HITGROUP_CHEST 2
#define HITGROUP_STOMACH 3
#define HITGROUP_LEFTARM 4
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG 6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR 10

void trace_line(Vector vecAbsStart, Vector vecAbsEnd, unsigned int mask, c_base_player* ignore, trace_t* ptr) {
	ray_t ray;
	ray.init(vecAbsStart, vecAbsEnd);
	c_tracefilter filter;
	filter.pSkip = ignore;
	g_engine_trace->trace_ray(ray, mask, &filter, ptr);
}
namespace autowall
{
	float get_hitgroup_damage_multiplier(int iHitGroup) {
		switch (iHitGroup) {
		case HITGROUP_GENERIC:
			return 0.5f;
		case HITGROUP_HEAD:
			return 2.0f;
		case HITGROUP_CHEST:
			return 0.5f;
		case HITGROUP_STOMACH:
			return 0.75f;
		case HITGROUP_LEFTARM:
			return 0.5f;
		case HITGROUP_RIGHTARM:
			return 0.5f;
		case HITGROUP_LEFTLEG:
			return 0.375f;
		case HITGROUP_RIGHTLEG:
			return 0.375f;
		case HITGROUP_GEAR:
			return 0.5f;
		default:
			return 1.0f;
		}
		return 1.0f;
	}

	void clip_trace_to_players(const Vector& vecAbsStart, const Vector& vecAbsEnd, uint32_t mask, i_tracefilter* filter, trace_t* tr) {
		float smallestFraction = tr->fraction;
		constexpr float maxRange = 60.0f;

		Vector delta(vecAbsEnd - vecAbsStart);
		const float delta_length = delta.Length();
		delta.NormalizeInPlace();

		ray_t ray;
		ray.init(vecAbsStart, vecAbsEnd);

		for (int i = 1; i <= g_global_vars->maxClients; ++i) {
			auto ent = c_base_player::get_player_by_index(i);
			if (!ent || ent->is_dormant() || ent->life_state() != LIFE_ALIVE)
				continue;

			if (filter && !filter->should_hit_entity(ent, mask))
				continue;

			auto collideble = ent->get_collideable();
			auto mins = collideble->obb_mins();
			auto maxs = collideble->obb_maxs();

			auto obb_center = (maxs + mins) * 0.5f;
			auto extend = (obb_center - vecAbsStart);
			auto rangeAlong = delta.Dot(extend);

			float range;
			if (rangeAlong >= 0.0f) {
				if (rangeAlong <= delta_length)
					range = Vector(obb_center - ((delta * rangeAlong) + vecAbsStart)).Length();
				else
					range = -(obb_center - vecAbsEnd).Length();
			}
			else {
				range = -extend.Length();
			}

			if (range >= 0.0f && range <= maxRange) {
				trace_t playerTrace;
				g_engine_trace->clip_ray_to_entity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, ent, &playerTrace);
				if (playerTrace.fraction < smallestFraction) {
					*tr = playerTrace;
					smallestFraction = playerTrace.fraction;
				}
			}
		}
	}

	void scale_damage(int hitgroup, c_base_player* enemy, float weapon_armor_ratio, float& current_damage) {
		static auto mp_damage_scale_ct_head = g_cvar->find_var("mp_damage_scale_ct_head");
		static auto mp_damage_scale_t_head = g_cvar->find_var("mp_damage_scale_t_head");
		static auto mp_damage_scale_ct_body = g_cvar->find_var("mp_damage_scale_ct_body");
		static auto mp_damage_scale_t_body = g_cvar->find_var("mp_damage_scale_t_body");

		auto team = enemy->team_num();
		auto head_scale = team == 2 ? mp_damage_scale_ct_head->get_float() : mp_damage_scale_t_head->get_float();
		auto body_scale = team == 2 ? mp_damage_scale_ct_body->get_float() : mp_damage_scale_t_body->get_float();

		auto armor_heavy = enemy->has_heavy_armor();
		auto armor_value = static_cast<float>(enemy->armor_value());

		if (armor_heavy)
			head_scale *= 0.5f;

		// ref: CCSPlayer::TraceAttack
		switch (hitgroup) {
		case HITGROUP_HEAD:
			current_damage = (current_damage * 4.f) * head_scale;
			break;
		case HITGROUP_CHEST:
		case 8:
			current_damage *= body_scale;
			break;
		case HITGROUP_STOMACH:
			current_damage = (current_damage * 1.25f) * body_scale;
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			current_damage *= body_scale;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			current_damage = (current_damage * 0.75f) * body_scale;
			break;
		default:
			break;
		}

		static auto IsArmored = [](c_base_player* player, int hitgroup) {
			auto has_helmet = player->has_helmet();
			auto armor_value = static_cast<float>(player->armor_value());

			if (armor_value > 0.f) {
				switch (hitgroup) {
				case HITGROUP_GENERIC:
				case HITGROUP_CHEST:
				case HITGROUP_STOMACH:
				case HITGROUP_LEFTARM:
				case HITGROUP_RIGHTARM:
				case 8:
					return true;
					break;
				case HITGROUP_HEAD:
					return has_helmet || player->has_heavy_armor();
					break;
				default:
					return player->has_heavy_armor();
					break;
				}
			}

			return false;
		};

		if (IsArmored(enemy, hitgroup)) {
			auto armor_scale = 1.f;
			auto armor_ratio = (weapon_armor_ratio * 0.5f);
			auto armor_bonus_ratio = 0.5f;

			if (armor_heavy) {
				armor_ratio *= 0.2f;
				armor_bonus_ratio = 0.33f;
				armor_scale = 0.25f;
			}

			float new_damage = current_damage * armor_ratio;
			float estiminated_damage = (current_damage - (current_damage * armor_ratio)) * (armor_scale * armor_bonus_ratio);
			if (estiminated_damage > armor_value)
				new_damage = (current_damage - (armor_value / armor_bonus_ratio));

			current_damage = new_damage;
		}
	}

	bool trace_to_exit(Vector& end, trace_t* enter_trace, Vector start, Vector dir, trace_t* exit_trace) {
		float distance = 0.0f;
		while (distance <= 90.0f) {
			distance += 4.0f;

			end = start + dir * distance;
			auto point_contents = g_engine_trace->get_point_contents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);
			if (point_contents & MASK_SHOT_HULL && !(point_contents & CONTENTS_HITBOX))
				continue;

			auto new_end = end - (dir * 4.0f);
			ray_t ray;
			ray.init(end, new_end);
			g_engine_trace->trace_ray(ray, MASK_SHOT, 0, exit_trace);
			if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX) {
				ray.init(end, start);
				c_tracefilter filter;
				filter.pSkip = exit_trace->hit_entity;
				g_engine_trace->trace_ray(ray, 0x600400B, &filter, exit_trace);
				if ((exit_trace->fraction < 1.0f || exit_trace->allsolid) && !exit_trace->startsolid) {
					end = exit_trace->endpos;
					return true;
				}
				continue;
			}

			if (!(exit_trace->fraction < 1.0 || exit_trace->allsolid || exit_trace->startsolid) || exit_trace->startsolid) {
				if (exit_trace->hit_entity) {
					if (enter_trace->hit_entity && enter_trace->hit_entity == g_entity_list->get_client_entity(0))
						return true;
				}
				continue;
			}

			if (exit_trace->surface.flags >> 7 & 1 && !(enter_trace->surface.flags >> 7 & 1))
				continue;

			if (exit_trace->plane.normal.Dot(dir) <= 1.0f) {
				auto fraction = exit_trace->fraction * 4.0f;
				end = end - (dir * fraction);
				return true;
			}
		}
		return false;
	}

	bool handle_bullet_penetration(c_cs_weapon_info* weaponInfo, fire_bullet_data_t& data) {
		surfacedata_t* enter_surface_data = g_phys_surface->get_surface_data(data.enter_trace.surface.surfaceProps);
		int enter_material = enter_surface_data->game.material;
		float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
		data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
		data.current_damage *= powf(weaponInfo->range_modifier, data.trace_length * 0.002f);

		if (data.trace_length > 3000.f || enter_surf_penetration_mod < 0.1f)
			data.penetrate_count = 0;

		if (data.penetrate_count <= 0)
			return false;

		Vector dummy;
		trace_t trace_exit;
		if (!trace_to_exit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
			return false;

		surfacedata_t* exit_surface_data = g_phys_surface->get_surface_data(trace_exit.surface.surfaceProps);
		int exit_material = exit_surface_data->game.material;
		float exit_surf_penetration_mod = *(float*)((uint8_t*)exit_surface_data + 76);
		float final_damage_modifier = 0.16f;
		float combined_penetration_modifier = 0.0f;
		if ((data.enter_trace.contents & CONTENTS_GRATE) != 0 || enter_material == 89 || enter_material == 71) {
			combined_penetration_modifier = 3.0f;
			final_damage_modifier = 0.05f;
		}
		else
			combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;

		if (enter_material == exit_material) {
			if (exit_material == 87 || exit_material == 85)
				combined_penetration_modifier = 3.0f;
			else if (exit_material == 76)
				combined_penetration_modifier = 2.0f;
		}

		float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
		float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / weaponInfo->penetration) * 1.25f);
		float thickness = (trace_exit.endpos - data.enter_trace.endpos).Length();

		thickness *= thickness;
		thickness *= v34;
		thickness /= 24.0f;

		float lost_damage = fmaxf(0.0f, v35 + thickness);
		if (lost_damage > data.current_damage)
			return false;

		if (lost_damage >= 0.0f)
			data.current_damage -= lost_damage;

		if (data.current_damage < 1.0f)
			return false;

		data.src = trace_exit.endpos;
		data.penetrate_count--;
		return true;
	}

	bool simulate_fire_bullet(c_base_combat_weapon* pWeapon, fire_bullet_data_t& data) {
		c_cs_weapon_info* weaponInfo = pWeapon->get_cs_weapondata();
		data.penetrate_count = 4;
		data.trace_length = 0.0f;
		data.current_damage = (float)weaponInfo->damage;
		while (data.penetrate_count > 0 && data.current_damage >= 1.0f) {
			data.trace_length_remaining = weaponInfo->range - data.trace_length;
			Vector end = data.src + data.direction * data.trace_length_remaining;
			trace_line(data.src, end, MASK_SHOT, g_local_player, &data.enter_trace);

			c_tracefilter local_filter;
			local_filter.pSkip = g_local_player;
			clip_trace_to_players(data.src, end + data.direction * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &local_filter, &data.enter_trace);
			if (data.enter_trace.fraction == 1.0f)
				break;

			if (data.enter_trace.hitgroup <= HITGROUP_RIGHTLEG && data.enter_trace.hitgroup > HITGROUP_GENERIC) {
				data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
				data.current_damage *= powf(weaponInfo->range_modifier, data.trace_length * 0.002f);

				c_base_player* player = (c_base_player*)data.enter_trace.hit_entity;
				scale_damage(data.enter_trace.hitgroup, player, weaponInfo->armor_ratio, data.current_damage);
				return true;
			}

			if (!handle_bullet_penetration(weaponInfo, data))
				break;
		}
		return false;
	}

	float get_damage(const Vector& point) {
		float damage = 0.f;
		Vector dst = point;
		fire_bullet_data_t data;
		data.src = g_local_player->get_eye_pos();
		data.filter.pSkip = g_local_player;
		data.direction = dst - data.src;
		data.direction.NormalizeInPlace();
		c_base_combat_weapon* activeWeapon = (c_base_combat_weapon*)g_entity_list->get_client_entity_from_handle(g_local_player->active_weapon());
		if (!activeWeapon)
			return -1.0f;
		if (simulate_fire_bullet(activeWeapon, data))
			damage = data.current_damage;
		return damage;
	}

}
