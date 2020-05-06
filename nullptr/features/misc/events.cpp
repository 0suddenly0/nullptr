#include "events.h"
#include "../changers/skin/skins.h"
#include "../visuals/visuals.h"
#include "../../options/globals.h"
#include <Windows.h>
#pragma comment(lib, "Winmm.lib") 


void c_events::init()
{
	g_game_events->add_listener(this, "player_hurt", false);
	g_game_events->add_listener(this, "player_death", false);
	g_game_events->add_listener(this, "bullet_impact", false);
	g_game_events->add_listener(this, "inferno_startburn", false);
	g_game_events->add_listener(this, "round_end", false);
	g_game_events->add_listener(this, "round_start", false);
	g_game_events->add_listener(this, "smokegrenade_detonate", false);
	g_game_events->add_listener(this, "round_prestart", false);
	g_game_events->add_listener(this, "round_freeze_end", false);
	g_game_events->add_listener(this, "item_purchase", false);
	g_game_events->add_listener(this, "bomb_beginplant", false);
	g_game_events->add_listener(this, "bomb_planted", false);
	g_game_events->add_listener(this, "bomb_begindefuse", false);
}

void c_events::shutdown()
{
	g_game_events->remove_listener(this);
}

void c_events::fire_game_event(i_game_event* event)
{
	if (strstr(event->get_name(), "player_hurt"))
	{
		c_base_player* hurt = (c_base_player*)g_entity_list->get_client_entity(g_engine_client->get_player_for_user_id(event->get_int("userid")));
		c_base_player* attacker = (c_base_player*)g_entity_list->get_client_entity(g_engine_client->get_player_for_user_id(event->get_int("attacker")));

		if (hurt && attacker)
		{		
			if(hurt != g_local_player && attacker == g_local_player && settings::misc::hitsound::enable)
			g_engine_client->execute_client_cmd(utils::snprintf("playvol buttons\\arena_switch_press_02.wav %.1f", settings::misc::hitsound::volume).c_str());

			bool hurt_dead = event->get_int("health") == 0;

			if (settings::visuals::damage_indicator::enable)
			{
				if (hurt != g_local_player && attacker == g_local_player)
				{
					damage_indicator_t DmgIndicator;
					DmgIndicator.damage = event->get_int("dmg_health");
					DmgIndicator.player = hurt;
					if (!hurt_dead)
						DmgIndicator.col = settings::visuals::damage_indicator::standart_color;
					else
						DmgIndicator.col = settings::visuals::damage_indicator::kill_color;
					DmgIndicator.hit_box = event->get_int("hitgroup");
					DmgIndicator.erase_time = g_global_vars->curtime + settings::visuals::damage_indicator::show_time;
					DmgIndicator.initialized = false;

					globals::damage_indicator::indicators.push_back(DmgIndicator);
				}
			}
		}
	}

	if (strstr(event->get_name(), "player_death"))
	{

		if (g_engine_client->get_player_for_user_id(event->get_int("attacker")) == g_engine_client->get_local_player())
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

			const auto icon_override = skins::get_icon_override(event->get_string("weapon"));

			if (icon_override)
			{
				event->set_string("weapon", icon_override);
			}
		}
	}

	if (strstr(event->get_name(), "bullet_impact"))
	{
		auto pl = (c_base_player*)g_entity_list->get_client_entity(g_engine_client->get_player_for_user_id(event->get_int("userid")));

		Vector position(event->get_float("x"), event->get_float("y"), event->get_float("z"));
		if (pl && pl->is_player())
		{
			if (pl == g_local_player)
			{
				if (settings::visuals::bullet_impact::line::local_player)
				{
					visuals::draw_bullet_tracer(pl->get_eye_pos(), position, settings::visuals::bullet_impact::line::color_local);
				}

				if (settings::visuals::bullet_impact::box::enable_s)
				{
					bullet_impact_t imp;
					imp.pos = position;
					imp.delete_time = g_global_vars->curtime + settings::visuals::bullet_impact::box::time_s;
					imp.size = settings::visuals::bullet_impact::box::size_s;
					imp.col = settings::visuals::bullet_impact::box::color_s;

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

	visuals::events(event);

	notify::events(event);
}

int c_events::get_event_debug_id(void)
{
	return EVENT_DEBUG_ID_INIT;
}

c_events events;