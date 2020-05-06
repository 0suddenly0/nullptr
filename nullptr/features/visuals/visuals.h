#pragma once

#include "../../render/render.h"
#include "../../helpers/math.h"
#include "../../valve_sdk/csgostructs.h"

#include <map>


struct molotov_info_t
{
	Vector position;
	float time_to_expire;
};

struct smoke_info_t
{
	Vector position;
	float time_to_expire;
};

struct sound_info_t {
	int guid;
	float soundTime;
	float alpha;
	Vector soundPos;
};

namespace visuals
{
	class player
	{
	public:
		struct
		{
			c_base_player* pl;
			bool          is_enemy;
			bool          is_visible;
			bool          is_localplayer;
			Color         clr;
			Vector        head_pos;
			Vector        feet_pos;
			RECT          bbox;
		} ctx;

		bool begin(c_base_player* pl);
		void draw_box();
		void draw_name();
		void draw_flags();
		void draw_weapon();
		void draw_health();
		void draw_armour();
		void draw_skeleton(c_base_entity* ent);
		void draw_backtrack();
	};

	std::vector<molotov_info_t> molotov_info;
	std::vector<smoke_info_t> smoke_info;

	void events(i_game_event* event);

	void draw_impacts(const c_effect_data& data);
	void draw_spreed_circle();
	void draw_aa_arrow();
	void draw_aa_lines();
	void draw_grenades();
	void draw_bullet_impacts();
	void nightmode();
	void draw_damage_indicator();
	void draw_bullet_tracer(Vector start, Vector end, Color col);
	void draw_granades(c_base_entity* entity);
	void draw_weapons(c_base_combat_weapon* ent);
	void draw_defusekit(c_base_entity* ent);
	void draw_window_c4(c_base_entity* ent);
	void draw_window_c4(c_base_entity* ent);
	void draw_items(c_base_entity* ent);
	void draw_hitmarker();
	void draw_spectator_list();
	void third_person();
	void draw_fov();
	void draw_player_arrow(c_base_player* pl);

	void render();

	void capsule_hitbox(c_base_entity* player, Color col, float duration);
}
