#include "../hooks.h"
#include "../../features/visuals/visuals.h"

namespace hooks
{
	bool __stdcall fire_event::hook(i_game_event* pEvent)
	{
		static auto oFireEvent = gameevents_vhook.get_original<fn>(index::fire_event);
		
		/*if (strstr(pEvent->get_name(), "player_hurt"))
		{
			c_base_player* hurt = (c_base_player*)g_entity_list->get_client_entity(g_engine_client->get_player_for_user_id(pEvent->get_int("userid")));
			c_base_player* attacker = (c_base_player*)g_entity_list->get_client_entity(g_engine_client->get_player_for_user_id(pEvent->get_int("attacker")));

			if (hurt && attacker)
			{
				bool hurt_dead = pEvent->get_int("health") == 0;

				if (settings::visuals::damage_indicator::enable)
				{
					if (hurt != g_local_player && attacker == g_local_player)
					{
						damage_indicator_t DmgIndicator;
						DmgIndicator.damage = pEvent->get_int("dmg_health");
						DmgIndicator.player = hurt;
						if (!hurt_dead)
							DmgIndicator.col = settings::visuals::damage_indicator::standart_color;
						else
							DmgIndicator.col = settings::visuals::damage_indicator::kill_color;
						DmgIndicator.hit_box = pEvent->get_int("hitgroup");
						DmgIndicator.erase_time = g_global_vars->curtime + settings::visuals::damage_indicator::show_time;
						DmgIndicator.initialized = false;

						globals::damage_indicator::indicators.push_back(DmgIndicator);
					}
				}
			}
		}

		if (strstr(pEvent->get_name(), "player_death"))
		{

			if (g_engine_client->get_player_for_user_id(pEvent->get_int("attacker")) == g_engine_client->get_local_player())
			{
				auto& weapon = g_local_player->active_weapon();
				if (weapon)
				{
					auto& skin_data = settings::changers::skin::m_items[weapon->item().item_definition_index()];
					auto& stat_track = settings::changers::skin::statrack_items[weapon->item().item_definition_index()];
					if (skin_data.enabled && skin_data.stat_trak)
					{

						stat_track.statrack_new.counter++;
						weapon->fallback_stat_trak() = stat_track.statrack_new.counter;
						weapon->get_client_networkable()->post_data_update(0);
						weapon->get_client_networkable()->on_data_changed(0);
					}
				}

				const auto icon_override = skins::get_icon_override(pEvent->get_string("weapon"));

				if (icon_override)
				{
					pEvent->set_string("weapon", icon_override);
				}
			}
		}

		if (strstr(pEvent->get_name(), "bullet_impact"))
		{
			auto pl = (c_base_player*)g_entity_list->get_client_entity(g_engine_client->get_player_for_user_id(pEvent->get_int("userid")));

			Vector position(pEvent->get_float("x"), pEvent->get_float("y"), pEvent->get_float("z"));
			if (pl && pl->is_player())
			{
				if (pl == g_local_player)
				{
					if (settings::visuals::bullet_impact::line::local_player)
					{
						visuals::draw_bullet_tracer(pl->get_eye_pos(), position, settings::visuals::bullet_impact::line::color_local);
					}

					if (settings::visuals::bullet_impact::box::enable)
					{
						bullet_impact_t imp;
						imp.pos = position;
						imp.delete_time = g_global_vars->curtime + settings::visuals::bullet_impact::box::time;
						imp.size = settings::visuals::bullet_impact::box::size;
						imp.col = settings::visuals::bullet_impact::box::color;

						globals::bullet_impact::bullet_impacts.push_back(imp);
					}
				}

				if (pl->team_num() == g_local_player->team_num() && pl != g_local_player && settings::visuals::bullet_impact::line::teammates)
				{
					visuals::draw_bullet_tracer(pl->get_eye_pos(), position, settings::visuals::bullet_impact::line::color_teammates);
				}

				if (pl->team_num() != g_local_player->team_num() && settings::visuals::bullet_impact::line::enemies)
				{
					visuals::draw_bullet_tracer(pl->get_eye_pos(), position, settings::visuals::bullet_impact::line::color_enemies);
				}
			}
		}	

		visuals::events(pEvent);

		//notify::notify_events(pEvent);*/

		return oFireEvent(g_game_events, pEvent);
	}
}