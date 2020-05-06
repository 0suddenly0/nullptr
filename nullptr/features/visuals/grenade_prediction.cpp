#include "grenade_prediction.h"
#include "../../options/options.h"
#include "../../valve_sdk/csgostructs.h"
#include "../../helpers/math.h"
#include "../../helpers/utils.h"
#include "../../valve_sdk/sdk.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define Assert( _exp ) ((void)0)

float CSGO_Armor(float flDamage, int ArmorValue) {
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue)* (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

namespace grenade_prediction
{
	void tick(int buttons)
	{
		if (!settings::visuals::grenade_prediction::enable)
			return;

		bool in_attack = (buttons & IN_ATTACK);
		bool in_attack2 = (buttons & IN_ATTACK2);

		act = (in_attack && in_attack2) ? ACT_DROP :
			(in_attack2) ? ACT_THROW :
			(in_attack) ? ACT_LOB :
			ACT_NONE;
	}

	void view()
	{
		if (!settings::visuals::grenade_prediction::enable)
			return;

		if (!g_local_player)
			return;

		auto weapon = g_local_player->active_weapon().get();
		if (!weapon)
			return;

		if ((weapon->is_grenade()) && act != ACT_NONE)
		{
			QAngle Angles;
			g_engine_client->get_view_angles(&Angles);


			type = static_cast<int>(weapon->item().item_definition_index());
			simulate(Angles, g_local_player);

		}
	}

	void paint()
	{
		if (!settings::visuals::grenade_prediction::enable)
			return;

		if (!g_local_player)
			return;

		auto weapon = g_local_player->active_weapon().get();
		if (!weapon)
			return;

		if ((type) && path.size() > 1 && other_collisions.size() > 0 && act != ACT_NONE && weapon->is_grenade())
		{
			Vector ab, cd;
			Vector prev = path[0];
			for (auto it = path.begin(), end = path.end(); it != end; ++it)
			{
				if (math::world_to_screen(prev, ab) && math::world_to_screen(*it, cd))
				{
					render::draw_line(ab[0], ab[1], cd[0], cd[1], settings::visuals::grenade_prediction::main, settings::visuals::grenade_prediction::line_thickness);
				}
				prev = *it;
			}

			for (auto it = other_collisions.begin(), end = other_collisions.end() - 1; it != end; ++it)
			{
				draw_3d_cube(settings::visuals::grenade_prediction::colision_box_size, it->second, it->first, Color(settings::visuals::grenade_prediction::main_box));
			}

			draw_3d_cube(settings::visuals::grenade_prediction::main_colision_box_size, other_collisions.rbegin()->second, other_collisions.rbegin()->first, settings::visuals::grenade_prediction::end_box);

			if (settings::visuals::grenade_prediction::radius)
			{
				render::draw_circle_3d(other_collisions.rbegin()->first, 50, get_radius(), settings::visuals::grenade_prediction::radius_color);
			}

			std::string EntName;
			auto bestdmg = 0;
			static Color redcol = { 255, 0, 0, 255 };
			static Color greencol = { 25, 255, 25, 255 };
			static Color yellowgreencol = { 177, 253, 2, 255 };
			static Color yellowcol = { 255, 255, 0, 255 };
			static Color orangecol = { 255, 128, 0, 255 };
			static Color white = { 255,2555,255 };
			Color* BestColor = &redcol;

			Vector endpos = path[path.size() - 1];
			Vector absendpos = endpos;

			float totaladded = 0.0f;

			while (totaladded < 30.0f) {
				if (g_engine_trace->get_point_contents(endpos) == CONTENTS_EMPTY)
					break;

				totaladded += 2.0f;
				endpos.z += 2.0f;
			}

			c_base_combat_weapon* pWeapon = g_local_player->active_weapon().get();
			auto weapon = g_local_player->active_weapon();
			int weap_id = weapon->item().item_definition_index();

			type = weap_id;

			if (pWeapon &&
				weap_id == WEAPON_HEGRENADE ||
				weap_id == WEAPON_MOLOTOV ||
				weap_id == WEAPON_INCGRENADE) {
				for (int i = 1; i < 64; i++) {
					c_base_player* pEntity = (c_base_player*)g_entity_list->get_client_entity(i);


					if (!pEntity || pEntity->team_num() == g_local_player->team_num())
						continue;

					float dist = (pEntity->vec_origin() - endpos).Length();

					if (dist < 350.0f) {
						c_tracefilter filter;
						filter.pSkip = g_local_player;
						ray_t ray;
						Vector NadeScreen;
						math::world_to_screen(endpos, NadeScreen);

						Vector vPelvis = pEntity->get_bone_pos(HITBOX_PELVIS);
						ray.init(endpos, vPelvis);
						trace_t ptr;
						g_engine_trace->trace_ray(ray, MASK_SHOT, &filter, &ptr);

						if (ptr.hit_entity == pEntity) {
							Vector PelvisScreen;

							math::world_to_screen(vPelvis, PelvisScreen);

							static float a = 105.0f;
							static float b = 25.0f;
							static float c = 140.0f;

							float d = ((((pEntity->vec_origin()) - prev).Length() - b) / c);
							float flDamage = a * exp(-d * d);
							auto dmg = max(static_cast<int>(ceilf(CSGO_Armor(flDamage, pEntity->armor_value()))), 0);

							Color* destcolor = dmg >= 65 ? &redcol : dmg >= 40 ? &orangecol : dmg >= 20 ? &white : &greencol;

							if (dmg > bestdmg)
							{
								EntName = pEntity->get_name();
								BestColor = destcolor;
								bestdmg = dmg;
							}
						}
					}
				}
			}
		}
	}

	void setup(c_base_player* pl, Vector& vecSrc, Vector& vecThrow, const QAngle& angEyeAngles)
	{
		QAngle angThrow = angEyeAngles;
		float pitch = angThrow.pitch;

		if (pitch <= 90.0f)
		{
			if (pitch < -90.0f)
			{
				pitch += 360.0f;
			}
		}
		else
		{
			pitch -= 360.0f;
		}
		float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
		angThrow.pitch = a;

		// Gets ThrowVelocity from weapon files
		// Clamped to [15,750]
		float flVel = 750.0f * 0.9f;

		// Do magic on member of grenade object [esi+9E4h]
		// m1=1  m1+m2=0.5  m2=0
		static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
		float b = power[act];
		// Clamped to [0,1]
		b = b * 0.7f;
		b = b + 0.3f;
		flVel *= b;

		Vector vForward, vRight, vUp;
		math::angle_vectors(angThrow, vForward, vRight, vUp);

		vecSrc = pl->vec_origin();
		vecSrc += pl->view_offset();
		float off = (power[act] * 12.0f) - 12.0f;
		vecSrc.z += off;

		// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
		trace_t tr;
		Vector vecDest = vecSrc;
		vecDest.MulAdd(vecDest, vForward, 22.0f);
		trace_hull(vecSrc, vecDest, tr);

		// After the hull trace it moves 6 units back along vForward
		// vecSrc = tr.endpos - vForward * 6
		Vector vecBack = vForward; vecBack *= 6.0f;
		vecSrc = tr.endpos;
		vecSrc -= vecBack;

		// Finally calculate velocity
		vecThrow = pl->velocity(); vecThrow *= 1.25f;
		vecThrow.MulAdd(vecThrow, vForward, flVel);
	}

	void simulate(QAngle& Angles, c_base_player* pLocal)
	{
		Vector vecSrc, vecThrow;
		setup(pLocal, vecSrc, vecThrow, Angles);

		float interval = g_global_vars->interval_per_tick;

		// Log positions 20 times per sec
		int logstep = static_cast<int>(0.05f / interval);
		int logtimer = 0;

		path.clear();
		other_collisions.clear();
		for (unsigned int i = 0; i < path.max_size() - 1; ++i)
		{
			if (!logtimer)
				path.push_back(vecSrc);

			int s = step(vecSrc, vecThrow, i, interval);
			if ((s & 1) || vecThrow == Vector(0, 0, 0))
				break;

			// Reset the log timer every logstep OR we bounced
			if ((s & 2) || logtimer >= logstep) logtimer = 0;
			else ++logtimer;
		}
		path.push_back(vecSrc);
	}

	int step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
	{
		// Apply gravity
		Vector move;
		add_gravity_move(move, vecThrow, interval, false);

		// Push entity
		trace_t tr;
		push_entity(vecSrc, move, tr);

		int result = 0;
		// Check ending conditions
		if (check_detonate(vecThrow, tr, tick, interval))
		{
			result |= 1;
		}

		// Resolve collisions
		if (tr.fraction != 1.0f)
		{
			result |= 2; // Collision!
			resolve_fly_collision_custom(tr, vecThrow, interval);
			QAngle angles;
			math::vector_angles((tr.endpos - tr.startpos).Normalized(), angles);
			other_collisions.push_back(std::make_pair(tr.endpos, angles));
		}

		if ((result & 1) || vecThrow == Vector(0, 0, 0))
		{
			QAngle angles;
			math::vector_angles((tr.endpos - tr.startpos).Normalized(), angles);
			other_collisions.push_back(std::make_pair(tr.endpos, angles));
		}

		// Set new position
		vecSrc = tr.endpos;

		return result;
	}

	float get_radius(int type_locl)
	{
		switch (type_locl)
		{
		case (int)WEAPON_SMOKEGRENADE:
			return 114.f;

		case (int)WEAPON_MOLOTOV:
		case (int)WEAPON_INCGRENADE:
			return 150.f;

		case (int)WEAPON_HEGRENADE:
			return 250.f;
		
		default:
			Assert(false);
			return false;
		}
	}

	bool check_detonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
	{
		firegrenade_didnt_hit = false;
		switch (type)
		{
		case (int)WEAPON_SMOKEGRENADE:
		case (int)WEAPON_DECOY:
			// Velocity must be <0.1, this is only checked every 0.2s
			if (vecThrow.Length() < 0.1f)
			{
				int det_tick_mod = static_cast<int>(0.2f / interval);
				return !(tick % det_tick_mod);
			}
			return false;

			/* TIMES AREN'T COMPLETELY RIGHT FROM WHAT I'VE SEEN ! ! ! */
		case (int)WEAPON_MOLOTOV:
		case (int)WEAPON_INCGRENADE:
			// Detonate when hitting the floor
			if (tr.fraction != 1.0f && tr.plane.normal.z > 0.7f)
				return true;
			// OR we've been flying for too long

		case (int)WEAPON_FLASHBANG:
		case (int)WEAPON_HEGRENADE:
		{
			// Pure timer based, detonate at 1.5s, checked every 0.2s
			firegrenade_didnt_hit = static_cast<float>(tick)* interval > 1.5f && !(tick % static_cast<int>(0.2f / interval));
			return firegrenade_didnt_hit;
		}
		default:
			Assert(false);
			return false;
		}
	}

	void trace_hull(Vector& src, Vector& end, trace_t& tr)
	{
		// Setup grenade hull
		static const Vector hull[2] = { Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f) };

		c_tracefilter filter;
		filter.set_ignore_class("BaseCSGrenadeProjectile");
		filter.set_ignore_class("SmokeGrenadeProjectile");
		filter.set_ignore_class("MolotovProjectile");
		filter.set_ignore_class("DecoyProjectile");

		filter.pSkip = g_entity_list->get_client_entity(g_engine_client->get_local_player());

		ray_t ray;
		ray.init(src, end, hull[0], hull[1]);

		const unsigned int mask = 0x200400B;
		g_engine_trace->trace_ray(ray, mask, &filter, &tr);
	}

	void add_gravity_move(Vector& move, Vector& vel, float frametime, bool onground)
	{
		Vector basevel(0.0f, 0.0f, 0.0f);

		move.x = (vel.x + basevel.x) * frametime;
		move.y = (vel.y + basevel.y) * frametime;

		if (onground)
		{
			move.z = (vel.z + basevel.z) * frametime;
		}
		else
		{
			// Game calls GetActualGravity( this );
			float gravity = 800.0f * 0.4f;

			float newZ = vel.z - (gravity * frametime);
			move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

			vel.z = newZ;
		}
	}

	void push_entity(Vector& src, const Vector& move, trace_t& tr)
	{
		Vector vecAbsEnd = src;
		vecAbsEnd += move;

		// Trace through world
		trace_hull(src, vecAbsEnd, tr);
	}

	void resolve_fly_collision_custom(trace_t& tr, Vector& vecVelocity, float interval)
	{
		// Calculate elasticity
		float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
		float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
		float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
		if (flTotalElasticity > 0.9f) flTotalElasticity = 0.9f;
		if (flTotalElasticity < 0.0f) flTotalElasticity = 0.0f;

		// Calculate bounce
		Vector vecAbsVelocity;
		physics_clip_velocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
		vecAbsVelocity *= flTotalElasticity;

		// Stop completely once we move too slow
		float flSpeedSqr = vecAbsVelocity.LengthSqr();
		static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
		if (flSpeedSqr < flMinSpeedSqr)
			vecAbsVelocity.Zero();

		// Stop if on ground
		if (tr.plane.normal.z > 0.7f)
		{
			vecVelocity = vecAbsVelocity;
			vecAbsVelocity.Mul((1.0f - tr.fraction) * interval);
			push_entity(tr.endpos, vecAbsVelocity, tr);
		}
		else
		{
			vecVelocity = vecAbsVelocity;
		}
	}

	int physics_clip_velocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
	{
		static const float STOP_EPSILON = 0.1f;

		float    backoff;
		float    change;
		float    angle;
		int        i, blocked;

		blocked = 0;

		angle = normal[2];

		if (angle > 0)
		{
			blocked |= 1;        // floor
		}
		if (!angle)
		{
			blocked |= 2;        // step
		}

		backoff = in.Dot(normal) * overbounce;

		for (i = 0; i < 3; i++)
		{
			change = normal[i] * backoff;
			out[i] = in[i] - change;
			if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			{
				out[i] = 0;
			}
		}

		return blocked;
	}

	void draw_3d_cube(float scalar, QAngle angles, Vector middle_origin, Color outline)
	{
		Vector pos;

		if (!math::world_to_screen(middle_origin, pos))
			return;

		render::draw_box_filled(pos.x - scalar, pos.y - scalar, pos.x + scalar, pos.y + scalar, outline);
	}

}
