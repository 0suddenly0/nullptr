#include <algorithm>

#include "../notify/notify.h"
#include "../../menu/menu.h"

#include "visuals.h"

float flHurtTime;

#include "../../options/options.h"
#include "../../helpers/math.h"
#define M_PI			3.14159265358979323846
#define DEG2RAD2( x  )  ( (float)(x) * (float)(PI_F / 180.f) )
#include "../../helpers/utils.h"

#include "grenade_prediction.h"
//#include "backtrack.h"
//#include "aimbot.h"
#include "../../hooks/hooks.h"

int selected_entity;

auto global_esp_settings = settings::visuals::esp::esp_items[selected_entity];

float fl_WeaponMaxAmmo[1024];

float flPlayerAlpha[65];
float PlayerAlpha[65];
float StoredCurtimePlayerAlpha[1024];
float StoredCurtimePlayer[1024];

std::map<int, std::string> fixed_names =
{
	//pistols
	////////////////////////////////////////////////
	{ ItemDefinitionIndex::WEAPON_USP_SILENCER, "usp" },
	{ ItemDefinitionIndex::WEAPON_GLOCK, "glock" },
	{ ItemDefinitionIndex::WEAPON_HKP2000, "p2000" },
	{ ItemDefinitionIndex::WEAPON_ELITE, "elite" },
	{ ItemDefinitionIndex::WEAPON_P250, "p250" },
	{ ItemDefinitionIndex::WEAPON_FIVESEVEN, "five-seven" },
	{ ItemDefinitionIndex::WEAPON_CZ75A, "cz75a" },
	{ ItemDefinitionIndex::WEAPON_TEC9, "tec9" },
	{ ItemDefinitionIndex::WEAPON_DEAGLE, "deagle" },
	{ ItemDefinitionIndex::WEAPON_REVOLVER, "rovelver" },
	////////////////////////////////////////////////
	//

	//pp
	////////////////////////////////////////////////
    { ItemDefinitionIndex::WEAPON_MP9, "mp9" },
	{ ItemDefinitionIndex::WEAPON_MAC10, "mac10" },
	{ ItemDefinitionIndex::WEAPON_MP7, "mp7" },
	{ ItemDefinitionIndex::WEAPON_MP5, "mp5-sd" },
	{ ItemDefinitionIndex::WEAPON_UMP45, "ump45" },
	{ ItemDefinitionIndex::WEAPON_P90, "p90" },
	{ ItemDefinitionIndex::WEAPON_BIZON, "bizon" },
	////////////////////////////////////////////////
	//

	//rifles
	////////////////////////////////////////////////
	{ ItemDefinitionIndex::WEAPON_FAMAS, "famas" },
	{ ItemDefinitionIndex::WEAPON_M4A1_SILENCER, "m4a1-s" },
	{ ItemDefinitionIndex::WEAPON_M4A1, "m4a1" },
	{ ItemDefinitionIndex::WEAPON_SSG08, "ssg08" },
	{ ItemDefinitionIndex::WEAPON_AUG, "aug" },
	{ ItemDefinitionIndex::WEAPON_AWP, "awp" },
	{ ItemDefinitionIndex::WEAPON_SCAR20, "scar20" },
	{ ItemDefinitionIndex::WEAPON_GALILAR, "galil" },
	{ ItemDefinitionIndex::WEAPON_AK47, "ak 47" },
	{ ItemDefinitionIndex::WEAPON_SG556, "sg553" },
	{ ItemDefinitionIndex::WEAPON_G3SG1, "g3sg1" },
	////////////////////////////////////////////////
	//

	//have
	////////////////////////////////////////////////
	{ ItemDefinitionIndex::WEAPON_NOVA, "nova" },
	{ ItemDefinitionIndex::WEAPON_XM1014, "xm1014" },
	{ ItemDefinitionIndex::WEAPON_SAWEDOFF, "sawedoff" },
	{ ItemDefinitionIndex::WEAPON_M249, "m249" },
	{ ItemDefinitionIndex::WEAPON_NEGEV, "negev" },
	{ ItemDefinitionIndex::WEAPON_MAG7, "mag7" },
	////////////////////////////////////////////////
	//

	//granades
	////////////////////////////////////////////////
	{ ItemDefinitionIndex::WEAPON_FLASHBANG, "flash" },
	{ ItemDefinitionIndex::WEAPON_SMOKEGRENADE, "smoke" },
	{ ItemDefinitionIndex::WEAPON_MOLOTOV, "molotov" },
	{ ItemDefinitionIndex::WEAPON_INCGRENADE, "inc" },
	{ ItemDefinitionIndex::WEAPON_DECOY, "decoy" },
	{ ItemDefinitionIndex::WEAPON_HEGRENADE, "hae" },
	////////////////////////////////////////////////
	//


	//other
	////////////////////////////////////////////////
	{ ItemDefinitionIndex::WEAPON_C4, "c4" },
	{ ItemDefinitionIndex::WEAPON_KNIFE, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFEGG, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_BAYONET, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_FALCHION, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_FLIP, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_GHOST, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_GUT, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_NAVAJA, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_PUSH, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_STILETTO, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_T, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_TALON, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_URSUS, "knife" },
	{ ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER, "knife" },
	{ ItemDefinitionIndex::WEAPON_TASER, "zeus" }
	////////////////////////////////////////////////
	//
};


int bone(int event_bone)
{
	switch (event_bone)
	{
	case hit_head:
		return HITBOX_HEAD;
	case hit_left_foot:
		return HITBOX_RIGHT_THIGH;
	case hit_right_foot:
		return HITBOX_LEFT_THIGH;
	case hit_chest:
		return HITBOX_UPPER_CHEST;
	case hit_lower_chest:
		return HITBOX_LOWER_CHEST;
	case hit_left_hand:
		return HITBOX_RIGHT_FOREARM;
	case hit_right_hand:
		return HITBOX_LEFT_FOREARM;
	case hit_neck:
		return HITBOX_NECK;
	default:
		return HITBOX_STOMACH;
	}
}

int get_player_alpha(int alpha, int index)
{
	int i = alpha - flPlayerAlpha[index];
	i = std::clamp(i, 0, 255);
	return i;
}

struct spectator_t
{
	std::string name;
	observer_mode_t mode;
};

RECT get_bbox(c_base_entity* ent) {
	RECT rect{};
	auto collideable = ent->get_collideable();

	if (!collideable)
		return rect;

	auto min = collideable->obb_mins();
	auto max = collideable->obb_maxs();

	const matrix3x4_t& trans = ent->coordinate_frame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		math::vector_transform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!math::world_to_screen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

float dot_product(const Vector& a, const Vector& b)
{
	return (a.x * b.x
		+ a.y * b.y
		+ a.z * b.z);
}

mstudiobbox_t* get_hitbox(c_base_entity* entity, int hitbox_index)
{
	if (!entity)
		return NULL;
	if (entity->get_health() <= 0)
		return NULL;

	const auto pModel = entity->get_model();
	if (!pModel)
		return NULL;

	auto pStudioHdr = g_mdl_info->get_studiomodel(pModel);
	if (!pStudioHdr)
		return NULL;

	auto pSet = pStudioHdr->get_hitbox_set(0);
	if (!pSet)
		return NULL;

	if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
		return NULL;

	return pSet->get_hitbox(hitbox_index);
}

Vector get_hitbox_position(c_base_entity* entity, int hitbox_id)
{
	auto hitbox = get_hitbox(entity, hitbox_id);
	if (!hitbox)
		return Vector(0, 0, 0);

	auto bone_matrix = entity->get_bone_matrix(hitbox->bone);

	Vector bbmin, bbmax;
	math::vector_transform(hitbox->bbmin, bone_matrix, bbmin);
	math::vector_transform(hitbox->bbmax, bone_matrix, bbmax);

	return (bbmin + bbmax) * 0.5f;
}

void add_triangle_filled(std::array<Vector2D, 3>points, Color color)
{
	//std::vector<Vertex_t>vertices{ Vertex_t(points.at(0)), Vertex_t(points.at(1)), Vertex_t(points.at(2)) };
	std::vector<ImVec2>vertices{ ImVec2(points.at(0).x, points.at(0).y), ImVec2(points.at(1).x, points.at(1).y), ImVec2(points.at(2).x, points.at(2).y) };
	render::draw_textured_polygon(3, vertices, color);
}

float get_defuse_time(c_planted_c4* bomb)
{
	static float defuse_time = -1;

	if (!bomb)
		return 0;

	if (!bomb->m_hBombDefuser())
		defuse_time = -1;

	else if (defuse_time == -1)
		defuse_time = g_global_vars->curtime + bomb->defuse_length();

	if (defuse_time > -1 && bomb->m_hBombDefuser())
		return defuse_time - g_global_vars->curtime;

	return 0;
}

c_base_player* get_bomb_player()
{
	for (auto i = 1; i <= g_entity_list->get_highest_entity_index(); ++i)
	{
		auto* entity = c_base_player::get_player_by_index(i);
		if (!entity || entity->is_player() || entity->is_dormant() || entity == g_local_player)
			continue;

		if (entity->is_planted_c4())
			return entity;
	}

	return nullptr;
}

c_planted_c4* get_bomb()
{
	//if (!interfaces::game_rules_proxy || !interfaces::game_rules_proxy->m_bBombPlanted())
		//return nullptr;

	c_base_entity* entity;
	for (auto i = 1; i <= g_entity_list->get_max_entities(); ++i)
	{
		entity = c_base_entity::get_entity_by_index(i);
		if (entity && !entity->is_dormant() && entity->is_planted_c4())
			return reinterpret_cast<c_planted_c4*>(entity);
	}

	return nullptr;
}

observer_mode_t bec_person = observer_mode_t::OBS_MODE_IN_EYE;
bool setted_person = false;

namespace visuals
{
	bool player::begin(c_base_player* pl)
	{
		if (!pl->is_alive())
			return false;

		ctx.pl = pl;
		ctx.is_localplayer = g_local_player->ent_index() == pl->ent_index();
		ctx.is_enemy = g_local_player->team_num() != pl->team_num();
		ctx.is_visible = g_local_player->can_see_player(pl, HITBOX_CHEST);

		if (g_local_player->observer_target() == ctx.pl && !settings::misc::third_person::bind.enable)
		{
			return false;
		}

		if (ctx.is_enemy)
		{
			global_esp_settings = settings::visuals::esp::esp_items[esp_types::enemies];
		}
		else if (!ctx.is_enemy && !ctx.is_localplayer)
		{
			global_esp_settings = settings::visuals::esp::esp_items[esp_types::teammates];
		}
		else if (ctx.is_localplayer)
		{
			if (settings::misc::third_person::bind.enable)
				global_esp_settings = settings::visuals::esp::esp_items[esp_types::local_player];
			else
				return false;
		}

		if (!global_esp_settings.enable)
			return false;

		ctx.clr = ctx.is_visible ? global_esp_settings.box_visible : global_esp_settings.box_invisible;

		auto head = pl->get_hitbox_pos(HITBOX_HEAD);
		auto origin = pl->vec_origin();

		head.z += 6;

		if (!math::world_to_screen(head, ctx.head_pos) ||
			!math::world_to_screen(origin, ctx.feet_pos))
		{
			return false;
		}

		auto h = fabs(ctx.head_pos.y - ctx.feet_pos.y);
		auto w = h / 2.f;

		ctx.bbox.left = static_cast<long>(ctx.feet_pos.x - w * 0.45f);
		ctx.bbox.right = static_cast<long>(ctx.bbox.left + w);
		ctx.bbox.bottom = static_cast<long>(ctx.feet_pos.y);
		ctx.bbox.top = static_cast<long>(ctx.head_pos.y);

		return true;
	}

	void player::draw_skeleton(c_base_entity* ent)
	{
		auto model = ent->get_model();
		if (ent && !model)
			return;

		studiohdr_t* pStudioModel = g_mdl_info->get_studiomodel(model);

		if (pStudioModel) {
			static matrix3x4_t pBoneToWorldOut[128];

			if (ent->setup_bones(pBoneToWorldOut, 128, 256, 0.f)) {
				for (int i = 0; i < pStudioModel->numbones; i++) {
					mstudiobone_t* pBone = pStudioModel->get_bone(i);
					if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
						continue;

					Vector vBonePos1;
					if (!math::world_to_screen(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
						continue;

					Vector vBonePos2;
					if (!math::world_to_screen(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
						continue;


					if (ctx.is_visible)
						render::draw_line((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, Color(global_esp_settings.skeleton_visible, get_player_alpha(global_esp_settings.skeleton_visible.a(), ctx.pl->ent_index())));
					else
						render::draw_line((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, Color(global_esp_settings.skeleton_invisible, get_player_alpha(global_esp_settings.skeleton_invisible.a(), ctx.pl->ent_index())));
				}
			}
		}
	}

	void player::draw_backtrack()
	{
		Vector localEyePos = g_local_player->get_eye_pos();
		QAngle angles;
		int tick_count = -1;
		float best_fov = 180.0f;
		/*for (auto& node : g_Backtrack.data) 
		{
			auto& cur_data = node.second;
			if (cur_data.empty())
				return;

			for (auto& bd : cur_data) {
				float deltaTime = g_Backtrack.correct_time - (g_global_vars->curtime - bd.simTime);
				if (std::fabsf(deltaTime) > g_Aimbot.settings.backtrack_time)
					continue;

				Vector w2s;
				if (!Math::WorldToScreen(bd.hitboxPos, w2s))
					continue;

				Render::Get().RenderBoxFilled(w2s.x - 1.0f, w2s.y - 1.0f, w2s.x + 1.0f, w2s.y + 1.0f, Color(1.f, 1.f, 1.f, 1.f));
			}
		}*/
	}

	void player::draw_box()
	{
		Color main;
		if (ctx.pl->is_dormant())
			main = Color(global_esp_settings.dormant, get_player_alpha(255, ctx.pl->ent_index()));

		else if (!ctx.pl->is_dormant())
		{
			if (ctx.is_visible)
			{
				main = Color(global_esp_settings.box_visible, get_player_alpha(global_esp_settings.box_visible.a(), ctx.pl->ent_index()));
			}
			else if (!ctx.is_visible)
			{
				main = Color(global_esp_settings.box_invisible, get_player_alpha(global_esp_settings.box_invisible.a(), ctx.pl->ent_index()));
			}
		}
		Color outline = Color(0, 0, 0, get_player_alpha(255, ctx.pl->ent_index()));

		render::draw_box(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, main, 1);
		render::draw_box(ctx.bbox.left + 1, ctx.bbox.top + 1, ctx.bbox.right - 1, ctx.bbox.bottom - 1, outline, 1);
		render::draw_box(ctx.bbox.left - 1, ctx.bbox.top - 1, ctx.bbox.right + 1, ctx.bbox.bottom + 1, outline, 1);
	}

	void player::draw_health()
	{
		auto  hp = ctx.pl->health();
		float box_w;
		float box_h;
		float off = 7;
		int height;

		float x;
		float y;
		float w;
		float h;

		int green = int(hp * 2.55f);
		int red = 255 - green;

		Color main = global_esp_settings.health_based ? Color(red, green, 0, get_player_alpha(global_esp_settings.health_bar_main.a(), ctx.pl->ent_index())) : Color(global_esp_settings.health_bar_main, get_player_alpha(global_esp_settings.health_bar_main.a(), ctx.pl->ent_index()));

		if (global_esp_settings.health_position == 0)
		{
			box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);
			box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
			height = (box_h * hp) / 100;

			x = ctx.bbox.left - off;
			y = ctx.bbox.bottom;
			w = 4;
			h = box_h;
			render::draw_box(x, y - h - 1, x + w, y + 1, Color(global_esp_settings.health_bar_outline, get_player_alpha(global_esp_settings.health_bar_outline.a(), ctx.pl->ent_index())), 1.f, 0.f);
			render::draw_box_filled(x + 1, y - (int)box_h, x + w - 1, y, Color(global_esp_settings.health_bar_background, get_player_alpha(global_esp_settings.health_bar_background.a(), ctx.pl->ent_index())), 1.f, 0.f);

			render::draw_box_filled(x + 1, y - height, x + w - 1, y, main, 1.f, 0.f);
		}
		if (global_esp_settings.health_position == 1)
		{
			box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);
			box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);

			x = ctx.bbox.right + off;
			y = ctx.bbox.bottom;
			w = 4;
			h = box_h;

			height = (box_h * hp) / 100;

			//Render::Get().RenderBoxFilled(x - w + 1, (y - h), x - w + 2, (y - h) - 1, Color(g_Options.color_esp_healthbar_background, (int)flPlayerAlpha[ctx.pl->ent_index()]), 1.f, true);

			render::draw_box(x - w, (y - h) - 1, x, y + 1, Color(global_esp_settings.health_bar_outline, get_player_alpha(global_esp_settings.health_bar_outline.a(), ctx.pl->ent_index())), 1.f, true);
			render::draw_box_filled(x - w + 1, (y - h), x - 1, y, Color(global_esp_settings.health_bar_background, get_player_alpha(global_esp_settings.health_bar_background.a(), ctx.pl->ent_index())), 1.f, true);
			render::draw_box_filled(x - w + 1, (y - height), x - 1, y, main, 1.f, true);
		}

		if (global_esp_settings.health_position == 2)
		{
			box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);
			box_h = 4.f;

			x = ctx.bbox.left;
			y = ctx.bbox.top - off;
			w = 4;
			h = box_h;

			height = (box_w * hp) / 100;

			render::draw_box(x - 1.f, y, x + box_w + 1, y + w, Color(global_esp_settings.health_bar_outline, get_player_alpha(global_esp_settings.health_bar_outline.a(), ctx.pl->ent_index())), 1.f, 0.f);
			render::draw_box_filled(x, y + 1.f, x + box_w, y + w - 1.f, Color(global_esp_settings.health_bar_background, get_player_alpha(global_esp_settings.health_bar_background.a(), ctx.pl->ent_index())), 1.f, true);
			render::draw_box_filled(x, y + 1.f, x + height, y + w - 1.f, main, 1.f, true);
		}
		if (global_esp_settings.health_position == 3)
		{
			box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);
			box_h = 4.f;

			x = ctx.bbox.left;
			y = ctx.bbox.bottom + off;
			w = 4;
			h = box_h;

			height = (box_w * hp) / 100;

			render::draw_box(x - 1.f, y - w, x + box_w + 1, y, Color(global_esp_settings.health_bar_outline, get_player_alpha(global_esp_settings.health_bar_outline.a(), ctx.pl->ent_index())), 1.f, 0.f);
			render::draw_box_filled(x, y - w + 1.f, x + box_w, y - 1.f, Color(global_esp_settings.health_bar_background, get_player_alpha(global_esp_settings.health_bar_background.a(), ctx.pl->ent_index())), 1.f, true);
			render::draw_box_filled(x, y - w + 1.f, x + height, y - 1.f, main, 1.f, true);
		}

		if (global_esp_settings.health_in_bar && hp > 0 && hp < 100)
		{
			if (global_esp_settings.health_position == 0)
			{
				ImVec2 textSize = small_font->CalcTextSizeA(10, FLT_MAX, 0.0f, std::to_string(hp).c_str());
				render::draw_text(std::to_string(hp), ImVec2(x + 2, y - height - (textSize.y / 2)), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
			}
			if (global_esp_settings.health_position == 1)
			{
				ImVec2 textSize = small_font->CalcTextSizeA(10, FLT_MAX, 0.0f, std::to_string(hp).c_str());
				render::draw_text(std::to_string(hp), ImVec2(x - 2, y - height - (textSize.y / 2)), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
			}
			if (global_esp_settings.health_position == 2)
			{
				ImVec2 textSize = small_font->CalcTextSizeA(10, FLT_MAX, 0.0f, std::to_string(hp).c_str());
				render::draw_text(std::to_string(hp), ImVec2(x + height, y - 4), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
			}
			if (global_esp_settings.health_position == 3)
			{
				ImVec2 textSize = small_font->CalcTextSizeA(10, FLT_MAX, 0.0f, std::to_string(hp).c_str());
				render::draw_text(std::to_string(hp), ImVec2(x + height, y - 8), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
			}
		}
	}

	void player::draw_name()
	{
		player_info_t info = ctx.pl->get_player_info();

		auto sz = esp_font->CalcTextSizeA(12.f, FLT_MAX, 0.0f, info.szName);

		int offset = 2;

		bool armor_bar_work = global_esp_settings.armor_bar && ctx.pl->armor_value() > 0;

		if (global_esp_settings.health_position == 2 && global_esp_settings.health_bar)
		{
			if (ctx.pl->get_health() < 100 && global_esp_settings.health_in_bar)
			{
				offset += 2;
			}
			offset += 6;
		}

		if (global_esp_settings.armor_position == 2 && armor_bar_work && global_esp_settings.health_position == 2 && global_esp_settings.health_bar)
		{
			if ((global_esp_settings.armor_in_bar && ctx.pl->armor_value() < 100 && ctx.pl->armor_value() > 0) && (global_esp_settings.health_in_bar && ctx.pl->get_health() < 100))
				offset += 2;
		}

		if (global_esp_settings.armor_position == 2 && armor_bar_work)
		{
			if (ctx.pl->armor_value() < 100 && global_esp_settings.armor_in_bar)
			{
				offset += 2;
			}
			offset += 6;
		}

		render::draw_text(info.szName, ctx.feet_pos.x, ctx.head_pos.y - sz.y - offset, 12.f, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);

	}

	void player::draw_armour()
	{
		auto  armour = ctx.pl->armor_value();
		if (armour <= 0)
			return;

		float box_w;
		float box_h;
		float off = 7;
		int height;

		float x;
		float y;
		float w;
		float h;

		if (global_esp_settings.health_position == global_esp_settings.armor_position && global_esp_settings.health_bar)
		{
			off += 6;

			if (ctx.pl->health() < 100 && global_esp_settings.health_in_bar)
			{
				off += 2;
			}

			if (armour < 100 && global_esp_settings.armor_in_bar)
			{
				off += 2;
			}
		}

		Color main = Color(global_esp_settings.armor_bar_main, get_player_alpha(global_esp_settings.armor_bar_main.a(), ctx.pl->ent_index()));

		//"left" 0, "right" 1, "top" 2, "bottom" 3
		if (global_esp_settings.armor_position == 0)
		{
			box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);
			box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
			height = (box_h * armour) / 100;

			x = ctx.bbox.left - off;
			y = ctx.bbox.bottom;
			w = 4;
			h = box_h;
			//Color(g_Options.color_esp_weapons_ammobar_outline, (int)flPlayerAlpha[ctx.pl->ent_index()])
			render::draw_box(x, y - h - 1, x + w, y + 1, Color(global_esp_settings.armor_bar_outline, get_player_alpha(global_esp_settings.armor_bar_outline.a(), ctx.pl->ent_index())), 1.f, 0.f);
			render::draw_box_filled(x + 1, y - (int)box_h, x + w - 1, y, Color(global_esp_settings.armor_bar_background, get_player_alpha(global_esp_settings.armor_bar_background.a(), ctx.pl->ent_index())), 1.f, 0.f);
			render::draw_box_filled(x + 1, y - height, x + w - 1, y, main, 1.f, 0.f);
		}
		if (global_esp_settings.armor_position == 1)
		{
			box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);
			box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);

			x = ctx.bbox.right + off;
			y = ctx.bbox.bottom;
			w = 4;
			h = box_h;

			height = (box_h * armour) / 100;

			//Render::Get().RenderBoxFilled(x - w + 1, (y - h), x - w + 2, (y - h) - 1, Color(g_Options.color_esp_healthbar_background, (int)flPlayerAlpha[ctx.pl->ent_index()]), 1.f, true);

			render::draw_box(x - w, (y - h) - 1, x, y + 1, Color(global_esp_settings.armor_bar_outline, get_player_alpha(global_esp_settings.armor_bar_outline.a(), ctx.pl->ent_index())), 1.f, true);
			render::draw_box_filled(x - w + 1, (y - h), x - 1, y, Color(global_esp_settings.armor_bar_background, get_player_alpha(global_esp_settings.armor_bar_background.a(), ctx.pl->ent_index())), 1.f, true);
			render::draw_box_filled(x - w + 1, (y - height), x - 1, y, main, 1.f, true);
		}

		if (global_esp_settings.armor_position == 2)
		{
			box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);
			box_h = 4.f;

			x = ctx.bbox.left;
			y = ctx.bbox.top - off;
			w = 4;
			h = box_h;

			height = (box_w * armour) / 100;

			render::draw_box(x - 1.f, y, x + box_w + 1, y + w, Color(global_esp_settings.armor_bar_outline, get_player_alpha(global_esp_settings.armor_bar_outline.a(), ctx.pl->ent_index())), 1.f, 0.f);
			render::draw_box_filled(x, y + 1.f, x + box_w, y + w - 1.f, Color(global_esp_settings.armor_bar_background, get_player_alpha(global_esp_settings.armor_bar_background.a(), ctx.pl->ent_index())), 1.f, true);
			render::draw_box_filled(x, y + 1.f, x + height, y + w - 1.f, main, 1.f, true);
		}
		if (global_esp_settings.armor_position == 3)
		{
			box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);
			box_h = 4.f;

			x = ctx.bbox.left;
			y = ctx.bbox.bottom + off;
			w = 4;
			h = box_h;

			height = (box_w * armour) / 100;

			render::draw_box(x - 1.f, y - w, x + box_w + 1, y, Color(global_esp_settings.armor_bar_outline, get_player_alpha(global_esp_settings.armor_bar_outline.a(), ctx.pl->ent_index())), 1.f, 0.f);
			render::draw_box_filled(x, y - w + 1.f, x + box_w, y - 1.f, Color(global_esp_settings.armor_bar_background, get_player_alpha(global_esp_settings.armor_bar_background.a(), ctx.pl->ent_index())), 1.f, true);
			render::draw_box_filled(x, y - w + 1.f, x + height, y - 1.f, main, 1.f, true);
		}

		if (global_esp_settings.armor_in_bar && armour > 0 && armour < 100)
		{
			if (global_esp_settings.armor_position == 0)
			{
				ImVec2 textSize = small_font->CalcTextSizeA(10, FLT_MAX, 0.0f, std::to_string(armour).c_str());
				render::draw_text(std::to_string(armour), ImVec2(x + 2, y - height - (textSize.y / 2)), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
			}
			if (global_esp_settings.armor_position == 1)
			{
				ImVec2 textSize = small_font->CalcTextSizeA(10, FLT_MAX, 0.0f, std::to_string(armour).c_str());
				render::draw_text(std::to_string(armour), ImVec2(x - 2, y - height - (textSize.y / 2)), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
			}
			if (global_esp_settings.armor_position == 2)
			{
				ImVec2 textSize = small_font->CalcTextSizeA(10, FLT_MAX, 0.0f, std::to_string(armour).c_str());
				render::draw_text(std::to_string(armour), ImVec2(x + height, y - 4), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
			}
			if (global_esp_settings.armor_position == 3)
			{
				ImVec2 textSize = small_font->CalcTextSizeA(10, FLT_MAX, 0.0f, std::to_string(armour).c_str());
				render::draw_text(std::to_string(armour), ImVec2(x + height, y - 8), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
			}
		}
	}

	void player::draw_flags()
	{
		std::vector<std::string> flags;

		bool planting = ctx.pl->active_weapon()->get_item_definition_index() == ItemDefinitionIndex::WEAPON_C4 && ctx.pl->active_weapon()->started_arming();

		if (global_esp_settings.flags_flashed && ctx.pl->is_flashed(40.f))  flags.push_back("flashed");
		if (global_esp_settings.flags_defusing && ctx.pl->is_defusing()) flags.push_back("defusing");
		if (global_esp_settings.flags_scoped && ctx.pl->is_scoped())     flags.push_back("scoped");
		if (global_esp_settings.flags_bomb_carrier && ctx.pl->has_c4())     flags.push_back("bomb");
		if (global_esp_settings.flags_planting && planting)                flags.push_back("planting");

		std::string armor = "";

		if (global_esp_settings.flags_armor && ctx.pl->armor_value() > 0) armor += "A ";
		if (global_esp_settings.flags_helmet && ctx.pl->has_helmet())     armor += "H";

		if (armor != "")
			flags.push_back(armor);

		int x_offset = 4;

		bool armor_bar_work = global_esp_settings.armor_bar && ctx.pl->armor_value() > 0;

		if (global_esp_settings.health_position == 1 && global_esp_settings.health_bar)
		{
			if (ctx.pl->get_health() < 100 && global_esp_settings.health_in_bar)
			{
				x_offset += 2;
			}
			x_offset += 6;
		}

		if (global_esp_settings.armor_position == 1 && armor_bar_work && global_esp_settings.health_position == 1 && global_esp_settings.health_bar)
		{
			if ((global_esp_settings.armor_in_bar && ctx.pl->armor_value() < 100 && ctx.pl->armor_value() > 0) && (global_esp_settings.health_in_bar && ctx.pl->get_health() < 100))
				x_offset += 2;
		}

		if (global_esp_settings.armor_position == 1 && armor_bar_work)
		{
			if (ctx.pl->armor_value() < 100 && global_esp_settings.armor_in_bar)
			{
				if (global_esp_settings.health_position == 1 && global_esp_settings.health_bar && (!global_esp_settings.health_in_bar || ctx.pl->get_health() == 100))
					x_offset += 2;

				x_offset += 2;
			}

			x_offset += 6;
		}

		for (int i = 0; i < flags.size(); i++)
		{
			int cur_y = ctx.bbox.top + (i * 12);

			render::draw_text(flags[i], (int)ctx.bbox.right + x_offset, cur_y, 12.f, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())));
		}
	}

	void player::draw_weapon()
	{
		if (!g_local_player || !g_engine_client->is_connected())
			return;

		auto weapon = ctx.pl->active_weapon();

		if (!weapon) return;

		auto weapon_data = weapon->get_cs_weapondata();
		if (!weapon_data)
			return;

		int ammo = weapon->clip1();

		int global_offset = 7;

		bool armor_bar_work = global_esp_settings.armor_bar && ctx.pl->armor_value() > 0;

		if (global_esp_settings.health_position == 3 && global_esp_settings.health_bar)
		{
			global_offset += 6;
			if (global_esp_settings.health_in_bar && ctx.pl->get_health() < 100)
				global_offset += 2;
		}

		if (global_esp_settings.armor_position == 3 && armor_bar_work && global_esp_settings.health_position == 3 && global_esp_settings.health_bar)
		{
			if (global_esp_settings.health_bar)
			{
				if (ctx.pl->armor_value() < 100 && global_esp_settings.armor_in_bar)
				{
					global_offset += 2;
				}
			}
		}

		if (global_esp_settings.armor_position == 3 && armor_bar_work)
		{
			global_offset += 6;
			if (global_esp_settings.armor_in_bar && ctx.pl->armor_value() < 100)
				global_offset += 2;
		}

		auto animLayer = ctx.pl->get_anim_overlay(1);
		if (!animLayer->owner)
			return;

		auto activity = ctx.pl->get_sequence_activity(animLayer->sequence);

		bool reload_anim = activity == 967 && animLayer->weight != 0.f;

		if (weapon->clip1() != weapon_data->max_clip1 && weapon->clip1() > 0 || reload_anim)
			global_offset += 2;

		auto text = weapon_data->hud_name + 7;
		auto size = default_font->CalcTextSizeA(14.f, FLT_MAX, 0.0f, text);

		float text_pos = 3.f;
		int w = ctx.bbox.right - ctx.bbox.left;
		//Render::Get().RenderText(g_WeaponIcons[weapon->GetItemDefinitionIndex()], ctx.feet_pos.x, ctx.feet_pos.y + text_pos, 12.f, Color::White, true, g_pDefaultFont);
		if (global_esp_settings.weapon_ammo)
		{

			if (weapon->get_item_definition_index() != ItemDefinitionIndex::WEAPON_FLASHBANG &&
				weapon->get_item_definition_index() != ItemDefinitionIndex::WEAPON_SMOKEGRENADE &&
				weapon->get_item_definition_index() != ItemDefinitionIndex::WEAPON_MOLOTOV &&
				weapon->get_item_definition_index() != ItemDefinitionIndex::WEAPON_INCGRENADE &&
				weapon->get_item_definition_index() != ItemDefinitionIndex::WEAPON_DECOY &&
				weapon->get_item_definition_index() != ItemDefinitionIndex::WEAPON_HEGRENADE &&
				weapon->get_item_definition_index() != ItemDefinitionIndex::WEAPON_C4 &&
				weapon->get_item_definition_index() != ItemDefinitionIndex::WEAPON_KNIFE)
			{


				float box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);

				float width = 0.f;//(((box_w * weapon->clip1()) / weapon->GetCSWeaponData()->iMaxClip1));

				if (reload_anim)
				{
					float cycle = animLayer->cycle; // 1 = finished 0 = just started
					width = (((box_w * cycle) / 1.f));
				}
				else
				{
					width = (((box_w * weapon->clip1()) / weapon_data->max_clip1));
					//weapon->CurMaxReloadTime = 0.f;
				}


				//Render::Get().RenderBox(ctx.bbox.left - 1, ctx.bbox.bottom + 3 + global_offset, ctx.bbox.right + 1, ctx.bbox.bottom + 7 + global_offset, Color(global_esp_settings.ammo_bar_outline, (int)flPlayerAlpha[ctx.pl->ent_index()]));
				//Render::Get().RenderBoxFilled(ctx.bbox.left, ctx.bbox.bottom + 4 + global_offset, ctx.bbox.right, ctx.bbox.bottom + 6 + global_offset, Color(global_esp_settings.ammo_bar_background, (int)flPlayerAlpha[ctx.pl->ent_index()]));
				//Render::Get().RenderBoxFilled(ctx.bbox.left, ctx.bbox.bottom + 4 + global_offset, ctx.bbox.left + (int)width, ctx.bbox.bottom + 6 + global_offset, Color(global_esp_settings.ammo_bar_main, (int)flPlayerAlpha[ctx.pl->ent_index()]));

				box_w = (float)fabs(ctx.bbox.right - ctx.bbox.left);

				float x = ctx.bbox.left;
				float y = ctx.bbox.bottom + global_offset;
				float w = 4;

				render::draw_box(x - 1.f, y - w, x + box_w + 1, y, Color(global_esp_settings.ammo_bar_outline, get_player_alpha(global_esp_settings.ammo_bar_outline.a(), ctx.pl->ent_index())));
				render::draw_box_filled(x, y - w + 1.f, x + box_w, y - 1.f, Color(global_esp_settings.ammo_bar_background, get_player_alpha(global_esp_settings.ammo_bar_background.a(), ctx.pl->ent_index())));
				render::draw_box_filled(x, y - w + 1.f, x + width, y - 1.f, Color(global_esp_settings.ammo_bar_main, get_player_alpha(global_esp_settings.ammo_bar_main.a(), ctx.pl->ent_index())));

				text_pos = global_offset;

				int reload_percentage = (100 * animLayer->cycle) / 1.f;
				if (weapon->clip1() != weapon_data->max_clip1 && weapon->clip1() > 0 && activity != 967)
				{
					render::draw_text(std::to_string(weapon->clip1()), ImVec2(ctx.bbox.left + (int)width, ctx.bbox.bottom + global_offset - w - 4), 12.f, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
					text_pos = global_offset + 2;
				}
				else if (activity == 967 && reload_percentage != 99)
				{
					render::draw_text(std::to_string(reload_percentage) + "%", ImVec2(ctx.bbox.left + (int)width, ctx.bbox.bottom + global_offset - w - 4), 12.f, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);
					text_pos = global_offset + 2;
				}

			}
			else
			{
				if (global_offset == 7)
					text_pos = 3.f;
				else
					text_pos = global_offset - 7;
			}
		}
		else
		{
			if (global_offset == 7)
				text_pos = 3.f;
			else
				text_pos = global_offset - 7;
		}


		render::draw_text(fixed_names[weapon->get_item_definition_index()], ImVec2((ctx.bbox.left + w * 0.5f), ctx.bbox.bottom + text_pos), 12, Color(255, 255, 255, get_player_alpha(255, ctx.pl->ent_index())), true, true, esp_font);

	}

	void draw_granades(c_base_entity* entity)
	{
		if (!g_local_player)
			return;

		std::string name;
		Color clr;

		const model_t* model = entity->get_model();

		if (entity->get_client_class())
		{
			if (model)
			{
				studiohdr_t* hdr = g_mdl_info->get_studiomodel(model);
				if (hdr)
				{
					std::string hdrName = hdr->szName;
					if (hdrName.find("thrown") != std::string::npos || hdrName.find("dropped") != std::string::npos)
					{
						if (hdrName.find("flashbang") != std::string::npos)
						{
							name = "flash";
							clr = settings::visuals::grenades::color_flash;
						}
						else if (hdrName.find("fraggrenade") != std::string::npos)
						{
							name = "frag";
							clr = settings::visuals::grenades::color_frag;
						}
						else if (hdrName.find("molotov") != std::string::npos)
						{
							name = "molotov";
							clr = settings::visuals::grenades::color_molotov;
						}
						else if (hdrName.find("incendiarygrenade") != std::string::npos)
						{
							name = "incendiary";
							clr = settings::visuals::grenades::color_molotov;
						}
						else if (hdrName.find("decoy") != std::string::npos)
						{
							name = "decoy";
							clr = settings::visuals::grenades::color_decoy;
						}
						else if (hdrName.find("smoke") != std::string::npos)
						{
							name = "smoke";
							clr = settings::visuals::grenades::color_smoke;
						}
					}
				}

				if (!name.empty())
				{
					Vector pos;
					if (math::world_to_screen(entity->vec_origin(), pos))
					{
						render::draw_text(name, ImVec2(pos.x, pos.y), 11.f, clr, true, true, small_font);
					}
				}
			}
		}
	}

	void events(i_game_event* event)
	{
		if (strstr(event->get_name(), "inferno_startburn"))
		{
			Vector position(event->get_float("x"), event->get_float("y"), event->get_float("z"));
			molotov_info_t temp = { position, g_global_vars->curtime + 7.f };
			molotov_info.emplace_back(temp);
		}

		if (strstr(event->get_name(), "smokegrenade_detonate"))
		{
			Vector position(event->get_float("x"), event->get_float("y"), event->get_float("z"));
			smoke_info_t temp = { position, g_global_vars->curtime + 18.f };
			smoke_info.emplace_back(temp);
		}

		if (strstr(event->get_name(), "round_end") || strstr(event->get_name(), "round_prestart") || strstr(event->get_name(), "round_freeze_end"))
		{
			for (int i = 0; i < molotov_info.size(); i++)
				molotov_info.erase(molotov_info.begin() + i);

			for (int i = 0; i < smoke_info.size(); i++)
				smoke_info.erase(smoke_info.begin() + i);
		}

		if (!strcmp(event->get_name(), "player_hurt"))
		{
			c_base_player* hurt = (c_base_player*)g_entity_list->get_client_entity(g_engine_client->get_player_for_user_id(event->get_int("userid")));
			c_base_player* attacker = (c_base_player*)g_entity_list->get_client_entity(g_engine_client->get_player_for_user_id(event->get_int("attacker")));
			if (hurt != g_local_player && attacker == g_local_player)
			{
				flHurtTime = g_global_vars->realtime + 1.f;
			}

			bool hurt_dead = event->get_int("health") == 0;

			if (hurt != g_local_player && attacker == g_local_player)
			{
				if (settings::visuals::hitbox::enable && settings::visuals::hitbox::show_type == 0)
				{
					if (g_local_player && hurt)
					{
						capsule_hitbox(hurt, settings::visuals::hitbox::color, settings::visuals::hitbox::show_time);
					}
				}
				else if (settings::visuals::hitbox::enable && settings::visuals::hitbox::show_type == 1)
				{
					if (g_local_player && hurt && hurt_dead)
						capsule_hitbox(hurt, settings::visuals::hitbox::color, settings::visuals::hitbox::show_time);

				}
			}
		}
	}

	void draw_damage_indicator()
	{
		for (int i = 0; i < globals::damage_indicator::indicators.size(); i++)
		{
			auto& cur_indicator = globals::damage_indicator::indicators[i];

			if (cur_indicator.erase_time < g_global_vars->curtime)
			{
				globals::damage_indicator::indicators.erase(globals::damage_indicator::indicators.begin() + i);
				continue;
			}

			if (cur_indicator.erase_time - 1.f < g_global_vars->curtime)
			{
				cur_indicator.col._CColor[3] = utils::lerp(cur_indicator.col._CColor[3], 0, 0.1f);
			}

			if (!cur_indicator.initialized)
			{
				cur_indicator.position = cur_indicator.player->get_hitbox_pos(bone(cur_indicator.hit_box));
				if (!cur_indicator.player->is_alive())
				{
					cur_indicator.position.z -= 39.f;

					cur_indicator.position.z += settings::visuals::damage_indicator::offset_if_kill;
				}
				else
				{
					cur_indicator.position.z += settings::visuals::damage_indicator::offset_hit;
				}
				cur_indicator.end_position = cur_indicator.position + Vector(utils::random(-settings::visuals::damage_indicator::range_offset_x, settings::visuals::damage_indicator::range_offset_x), utils::random(-settings::visuals::damage_indicator::range_offset_x, settings::visuals::damage_indicator::range_offset_x), settings::visuals::damage_indicator::max_pos_y);

				cur_indicator.initialized = true;
			}

			if (cur_indicator.initialized)
			{
				cur_indicator.position.z = utils::lerp(cur_indicator.position.z, cur_indicator.end_position.z, settings::visuals::damage_indicator::speed);
				cur_indicator.position.x = utils::lerp(cur_indicator.position.x, cur_indicator.end_position.x, settings::visuals::damage_indicator::speed);
				cur_indicator.position.y = utils::lerp(cur_indicator.position.y, cur_indicator.end_position.y, settings::visuals::damage_indicator::speed);

				cur_indicator.last_update = g_global_vars->curtime;
			}

			Vector rend_pos;

			if (math::world_to_screen(cur_indicator.position, rend_pos))
			{
				render::draw_text("-" + std::to_string(cur_indicator.damage), rend_pos.x, rend_pos.y, settings::visuals::damage_indicator::text_size, cur_indicator.col, true);
			}
		}
	}

	void draw_weapons(c_base_combat_weapon* ent)
	{
		auto clean_item_name = [](const char* name) -> const char* {
			if (name[0] == 'C')
				name++;

			auto start = strstr(name, "Weapon");
			if (start != nullptr)
				name = start + 6;

			return name;
		};

		if (ent->owner_entity().is_valid())
			return;

		auto bbox = get_bbox(ent);

		if (bbox.right == 0 || bbox.bottom == 0)
			return;

		if (settings::visuals::dropped_weapon::box)
		{
			render::draw_box(RECT{ bbox.left - 1, bbox.top + 1, bbox.right + 1, bbox.bottom - 1 }, Color(0, 0, 0, 255));
			render::draw_box(bbox, settings::visuals::dropped_weapon::box_color);
			render::draw_box(RECT{ bbox.left + 1, bbox.top - 1, bbox.right - 1, bbox.bottom + 1 }, Color(0, 0, 0, 255));
		}

		auto name = clean_item_name(ent->get_client_class()->network_name);
		if (!name)
			return;

		int w = bbox.right - bbox.left;

		int offset = 3;

		bool is_grenade = ent->get_item_definition_index() == ItemDefinitionIndex::WEAPON_FLASHBANG ||
			ent->get_item_definition_index() == ItemDefinitionIndex::WEAPON_SMOKEGRENADE ||
			ent->get_item_definition_index() == ItemDefinitionIndex::WEAPON_MOLOTOV ||
			ent->get_item_definition_index() == ItemDefinitionIndex::WEAPON_INCGRENADE ||
			ent->get_item_definition_index() == ItemDefinitionIndex::WEAPON_DECOY ||
			ent->get_item_definition_index() == ItemDefinitionIndex::WEAPON_HEGRENADE;

		if (settings::visuals::dropped_weapon::ammo_bar)
		{
			if (!is_grenade &&
				ent->get_item_definition_index() != ItemDefinitionIndex::WEAPON_C4)
			{
				float box_w = (float)fabs(bbox.right - bbox.left);

				auto width = (((box_w * ent->clip1()) / ent->get_cs_weapondata()->max_clip1));

				render::draw_box(bbox.left - 1, bbox.top + 3, bbox.right + 1, bbox.top + 7, settings::visuals::dropped_weapon::bar_outline);

				render::draw_box_filled(bbox.left, bbox.top + 4, bbox.right, bbox.top + 6, settings::visuals::dropped_weapon::bar_background);

				render::draw_box_filled(bbox.left, bbox.top + 4, bbox.left + (int)width, bbox.top + 6, settings::visuals::dropped_weapon::bar_main);
				if (ent->clip1() != ent->get_cs_weapondata()->max_clip1 && ent->clip1() > 0)
				{
					render::draw_text(std::to_string(ent->clip1()), ImVec2(bbox.left + (int)width, bbox.top + 2), 12.f, Color(255, 255, 255, 255), true, true, esp_font);
					offset += 5;
				}
				offset += 4;
			}
		}

		if(!is_grenade)
		render::draw_text(fixed_names[ent->get_item_definition_index()], ImVec2((bbox.left + w * 0.5f), bbox.top + offset), 12, Color(255, 255, 255, 255), true, true, esp_font);
	}

	void draw_defusekit(c_base_entity* ent)
	{
		if (ent->owner_entity().is_valid())
			return;

		auto bbox = get_bbox(ent);

		if (bbox.right == 0 || bbox.bottom == 0)
			return;

		auto name = "defuse kit";
		auto sz = default_font->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
		int w = bbox.right - bbox.left;
		render::draw_text(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, Color(255, 255, 255, 255));
	}

	void draw_window_c4(c_base_entity* ent)
	{
		if (!ent)
			return;

		float bombTimer = ent->c4_blow() - g_global_vars->curtime;

		int x;
		int y;

		g_engine_client->get_screen_size(x, y);

		int windowX = 0;
		int windowY = y - 430;

		int windowSizeX = 86;
		static int windowSizeY = 15; // 3 item - 35 | 2 item - 25 | 1 item - 15
		const auto bomb = get_bomb();
		if (!bomb)
			return;

		if (bombTimer < 0)
			return;


		render::draw_box_filled(windowX, windowY, windowX + windowSizeX, windowY + windowSizeY, ImColor(0, 0, 0, 100));
		render::draw_box_filled(windowX, windowY, windowX + 2, windowY + windowSizeY, settings::misc::menu_color);

		render::draw_text("bomb: ", ImVec2(windowX + 4, windowY + 2), 11, Color(255, 255, 255, 255), false, false, small_font);
		ImVec2 textSize_bomb = small_font_2->CalcTextSizeA(11, FLT_MAX, 0.0f, "bomb: ");
		std::string bomb_timer_text = utils::snprintf("%4.3f", bombTimer);

		render::draw_text(bombTimer >= 0 ? bomb_timer_text : "0", ImVec2(windowX + 4 + textSize_bomb.x, windowY + 2), 11, Color(255, 255, 255, 255), false, false, small_font_2);

		ImVec2 textSize_defuse = small_font_2->CalcTextSizeA(11, FLT_MAX, 0.0f, "defuse: ");
		float time = get_defuse_time(bomb);

		std::string defuse_timer_text = utils::snprintf("%4.3f", time);

		const auto bomb_1 = get_bomb_player();

		if (!bomb_1)
			return;

		float flArmor = g_local_player->armor_value();
		float flDistance = g_local_player->get_eye_pos().DistTo(bomb_1->get_abs_origin());

		float a = 450.7f;
		float b = 75.68f;
		float c = 789.2f;
		float d = ((flDistance - b) / c);
		float flDamage = a * exp(-d * d);

		float flDmg = flDamage;

		if (flArmor > 0)
		{
			float flNew = flDmg * 0.5f;
			float flArmor = (flDmg - flNew) * 0.5f;

			if (flArmor > static_cast<float>(flArmor))
			{
				flArmor = static_cast<float>(flArmor)* (1.f / 0.5f);
				flNew = flDmg - flArmor;
			}

			flDamage = flNew;
		}

		std::string damage = utils::snprintf("%i", (int)flDamage);

		ImVec2 textSize_damage = small_font_2->CalcTextSizeA(11, FLT_MAX, 0.0f, "damage: ");

		if (get_defuse_time(bomb) > 0)
		{
			render::draw_text("defuse: ", ImVec2(windowX + 4, windowY + 12), 11, Color(255, 255, 255, 255), false, false, small_font_2);
			if (bombTimer < time)
			{
				render::draw_text(defuse_timer_text, ImVec2(windowX + 4 + textSize_defuse.x, windowY + 12), 11, Color(255, 50, 50, 255), false, false, small_font_2);
			}
			else if (bombTimer > time)
			{
				render::draw_text(defuse_timer_text, ImVec2(windowX + 4 + textSize_defuse.x, windowY + 12), 11, Color(50, 255, 50, 255), false, false, small_font_2);
			}
			if (flDamage > 1 && bombTimer >= 0)
			{
				render::draw_text("damage: ", ImVec2(windowX + 4, windowY + 22), 11, Color(255, 255, 255, 255), false, 0, small_font_2);
				render::draw_text(flDamage < 100 ? damage : "you dead", ImVec2(windowX + 4 + textSize_damage.x, windowY + 22), 11, Color(255, 255, 255, 255), false, false, small_font_2);
				windowSizeY = 35;
			}
			else
			{
				windowSizeY = 25;
			}

			float box_w = (float)fabs(0 - windowSizeX);

			float max_time;

			float width;

			if (globals::misc::bomb_defusing_with_kits)
			{
				width = (((box_w * time) / 5.f));
			}
			else
			{
				width = (((box_w * time) / 10.f));
			}

			render::draw_box_filled(windowX, windowY + windowSizeY + 2, windowX + (int)width, windowY + windowSizeY + 4, Color(50, 50, 255, 255));

		}
		else if (get_defuse_time(bomb) <= 0)
		{
			if (flDamage > 1 && bombTimer >= 0)
			{
				render::draw_text("damage: ", ImVec2(windowX + 4, windowY + 12), 11, Color(255, 255, 255, 255), false, false, small_font_2);
				render::draw_text(flDamage < 100 ? damage : "you dead", ImVec2(windowX + 4 + textSize_damage.x, windowY + 12), 11, Color(255, 255, 255, 255), false, false, small_font_2);
				windowSizeY = 25;
			}
			else
			{
				windowSizeY = 15;
			}
		}

		float box_w = (float)fabs(0 - windowSizeX);

		auto width = (((box_w * bombTimer) / 40.f));
		render::draw_box_filled(windowX, windowY + windowSizeY, windowX + (int)width, windowY + windowSizeY + 2, Color(255, 100, 100, 255));

	}

	void draw_items(c_base_entity* ent)
	{
		std::string itemstr = "Undefined";
		const model_t* itemModel = ent->get_model();
		if (!itemModel)
			return;
		studiohdr_t* hdr = g_mdl_info->get_studiomodel(itemModel);
		if (!hdr)
			return;
		itemstr = hdr->szName;
		if (itemstr.find("case_pistol") != std::string::npos)
			itemstr = "Pistol Case";
		else if (itemstr.find("case_light_weapon") != std::string::npos)
			itemstr = "Light Case";
		else if (itemstr.find("case_heavy_weapon") != std::string::npos)
			itemstr = "Heavy Case";
		else if (itemstr.find("case_explosive") != std::string::npos)
			itemstr = "Explosive Case";
		else if (itemstr.find("case_tools") != std::string::npos)
			itemstr = "Tools Case";
		else if (itemstr.find("random") != std::string::npos)
			itemstr = "Airdrop";
		else if (itemstr.find("dz_armor_helmet") != std::string::npos)
			itemstr = "Full Armor";
		else if (itemstr.find("dz_helmet") != std::string::npos)
			itemstr = "Helmet";
		else if (itemstr.find("dz_armor") != std::string::npos)
			itemstr = "Armor";
		else if (itemstr.find("upgrade_tablet") != std::string::npos)
			itemstr = "Tablet Upgrade";
		else if (itemstr.find("briefcase") != std::string::npos)
			itemstr = "Briefcase";
		else if (itemstr.find("parachutepack") != std::string::npos)
			itemstr = "Parachute";
		else if (itemstr.find("dufflebag") != std::string::npos)
			itemstr = "Cash Dufflebag";
		else if (itemstr.find("ammobox") != std::string::npos)
			itemstr = "Ammobox";
		else if (itemstr.find("dronegun") != std::string::npos)
			itemstr = "Turrel";

		auto bbox = get_bbox(ent);
		if (bbox.right == 0 || bbox.bottom == 0)
			return;
		auto sz = default_font->CalcTextSizeA(14.f, FLT_MAX, 0.0f, itemstr.c_str());
		int w = bbox.right - bbox.left;

		//Render::Get().RenderBox(bbox, g_Options.color_esp_item);
		render::draw_text(itemstr, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, Color(255, 255, 255));
	}

	void third_person()
	{
		if (!g_local_player)
			return;

		if (settings::misc::third_person::enable && settings::misc::third_person::bind.enable)
		{
			if (g_local_player->is_alive())
			{
				if (!g_input->m_fCameraInThirdPerson)
				{
					g_input->m_fCameraInThirdPerson = true;
				}

				float dist = settings::misc::third_person::dist;

				QAngle* view = g_local_player->get_vangles();
				trace_t tr;
				ray_t ray;

				Vector desiredCamOffset = Vector(cos(DEG2RAD(view->yaw)) * dist,
					sin(DEG2RAD(view->yaw)) * dist,
					sin(DEG2RAD(-view->pitch)) * dist
				);

				//cast a ray from the Current camera Origin to the Desired 3rd person Camera origin
				ray.init(g_local_player->get_eye_pos(), (g_local_player->get_eye_pos() - desiredCamOffset));
				c_tracefilter traceFilter;
				traceFilter.pSkip = g_local_player;
				g_engine_trace->trace_ray(ray, MASK_SHOT, &traceFilter, &tr);

				Vector diff = g_local_player->get_eye_pos() - tr.endpos;

				float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y));// Pythagorean

				bool horOK = distance2D > (dist - 2.0f);
				bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

				float cameraDistance;

				if (horOK && vertOK)  // If we are clear of obstacles
				{
					cameraDistance = dist; // go ahead and set the distance to the setting
				}
				else {
					if (vertOK) // if the Vertical Axis is OK
					{
						cameraDistance = distance2D * 0.95f;
					}
					else// otherwise we need to move closer to not go into the floor/ceiling
					{
						cameraDistance = abs(diff.z) * 0.95f;
					}
				}
				g_input->m_fCameraInThirdPerson = true;

				g_input->m_vecCameraOffset.z = cameraDistance;

				setted_person = false;
			}
			else
			{
				if (g_local_player->observer_target() != nullptr)
				{
					if (g_local_player->observer_mode() == observer_mode_t::OBS_MODE_IN_EYE)
					{
						bec_person = g_local_player->observer_mode();
						setted_person = true;
					}

					if (setted_person)
						g_local_player->observer_mode() = observer_mode_t::OBS_MODE_CHASE;
				}

				g_input->m_fCameraInThirdPerson = false;
			}
		}
		else
		{
			if (!g_local_player->is_alive())
			{
				if (setted_person)
					g_local_player->observer_mode() = bec_person;

				setted_person = false;
			}
			g_input->m_fCameraInThirdPerson = false;
		}
	}

	void nightmode()
	{
		std::string fallback_skybox = "";
		static int OldSky = 0;

		auto LoadNamedSky = reinterpret_cast<void(__fastcall*)(const char*)>(utils::pattern_scan(GetModuleHandleA("engine.dll"), "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));


		static bool OldNightmode;
		if (!g_engine_client->is_connected() || !g_engine_client->is_ingame() || !g_local_player || !g_local_player->is_alive())
		{
			fallback_skybox = "";
			OldNightmode = false;
			OldSky = 0;
			return;
		}

		static convar* r_DrawSpecificStaticProp;
		if (OldNightmode != settings::misc::nightmode)
		{

			r_DrawSpecificStaticProp = g_cvar->find_var("r_DrawSpecificStaticProp");
			r_DrawSpecificStaticProp->set_value(0);

			for (material_handle_t i = g_mat_system->first_material(); i != g_mat_system->invalid_material(); i = g_mat_system->next_material(i))
			{
				i_material* pMaterial = g_mat_system->get_material(i);
				if (!pMaterial)
					continue;
				if (strstr(pMaterial->get_texture_group_name(), "World") || strstr(pMaterial->get_texture_group_name(), "StaticProp"))
				{
					if (settings::misc::nightmode) {
						LoadNamedSky("sky_csgo_night02");

						if (strstr(pMaterial->get_texture_group_name(), "StaticProp"))
							pMaterial->color_modulate(0.11f, 0.11f, 0.11f);
						else
							pMaterial->color_modulate(0.05f, 0.05f, 0.05f);
					}
					else {
						LoadNamedSky("sky_cs15_daylight04_hdr");
						pMaterial->color_modulate(1.0f, 1.0f, 1.0f);
					}
				}
			}
			OldNightmode = settings::misc::nightmode;
		}

	}

	void draw_bullet_tracer(Vector start, Vector end, Color col)
	{
		if (!g_local_player || !settings::visuals::bullet_impact::line::enable)
		{
			return;
		}

		if (!start.IsValid() || !end.IsValid() || start.DistTo(end) < 0.1f)
			return;

		beam_info_t beamInfo;
		beamInfo.m_nType = TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/physbeam.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = settings::visuals::bullet_impact::line::time;
		beamInfo.m_flWidth = 0.5f;
		beamInfo.m_flEndWidth = 0.5f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = col.a();
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = col.r();
		beamInfo.m_flGreen = col.g();
		beamInfo.m_flBlue = col.b();
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = 0;//FBEAM_FADEIN | FBEAM_FADEOUT;
		beamInfo.m_vecStart = start;
		beamInfo.m_vecEnd = end;

		auto beam = g_beam->create_beam_points(beamInfo);

		if (beam)
		{
			g_beam->draw_beam(beam);
		}

	}

	void draw_player_arrow(c_base_player* pl)
	{
		if (!settings::visuals::ofc::enable)
			return;

		auto local_player = g_local_player;
		if (!local_player) return;

		auto entity = pl;
		if (!entity || !entity->is_player() || entity->life_state() != LIFE_ALIVE
			|| (entity->team_num() == local_player->team_num()))
			return;

		auto local = g_local_player;

		const int fade = (int)((4 * g_global_vars->frametime) * 255);

		if (!local)
			return;

		if (!local->is_alive())
			return;

		if (!entity)
			return;

		Vector viewangles;
		QAngle viewangles_angle;
		int width, height;

		g_engine_client->get_view_angles(&viewangles_angle);
		viewangles.y = viewangles_angle.yaw;
		viewangles.x = viewangles_angle.pitch;
		viewangles.z = viewangles_angle.roll;
		g_engine_client->get_screen_size(width, height);

		const auto screen_center = Vector2D(width * .5f, height * .5f);

		const auto angle_yaw_rad = DEG2RAD(viewangles.y - math::calc_angle(local->get_eye_pos(), get_hitbox_position(entity, 2)).yaw - 90);

		int radius = settings::visuals::ofc::range;
		int size = settings::visuals::ofc::hight;
		int size_x = settings::visuals::ofc::wight;
		int size_y = settings::visuals::ofc::hight;

		const auto new_point_x = screen_center.x + ((((width - (size * 3)) * .5f) * (radius / 100.0f)) * cos(angle_yaw_rad)) + (int)(6.0f * (((float)size - 4.f) / 16.0f));
		const auto new_point_y = screen_center.y + ((((height - (size * 3)) * .5f) * (radius / 100.0f)) * sin(angle_yaw_rad));

		std::array< Vector2D, 3 >points{
			Vector2D(new_point_x - size_y, new_point_y - size_x),
			Vector2D(new_point_x + size_y, new_point_y),
			Vector2D(new_point_x - size_y, new_point_y + size_x)
		};

		math::rotate_triangle(points, viewangles.y - math::calc_angle(local->get_eye_pos(), get_hitbox_position(entity, 2)).yaw - 90);
		add_triangle_filled(points, Color(settings::visuals::ofc::color, (int)PlayerAlpha[entity->ent_index()]));
	}

	void capsule_hitbox(c_base_entity* pPlayer, Color col, float duration)
	{
		/*if (!pPlayer)
			return;

		studiohdr_t* pStudioModel = g_MdlInfo->GetStudiomodel(pPlayer->GetModel());

		if (!pStudioModel)
			return;

		mstudiohitboxset_t* pHitboxSet = pStudioModel->GetHitboxSet(0);

		if (!pHitboxSet)
			return;

		for (int i = 0; i < pHitboxSet->numhitboxes; i++)
		{
			mstudiobbox_t* pHitbox = pHitboxSet->GetHitbox(i);

			if (!pHitbox)
				continue;

			auto bone_matrix = pPlayer->GetBoneMatrix(pHitbox->bone);
			Vector vMin, vMax;

			Math::VectorTransform(pHitbox->bbmin, bone_matrix, vMin);
			Math::VectorTransform(pHitbox->bbmax, bone_matrix, vMax);

			if (pHitbox->m_flRadius > -1)
			{
				g_debug_overlay->AddCapsuleOverlay(vMin, vMax, pHitbox->m_flRadius, col.r(), col.g(), col.b(), col.a(), duration);
			}
		}*/

		/*if (!pPlayer)
			return;

		studiohdr_t* pStudioModel = g_MdlInfo->GetStudiomodel((model_t*)pPlayer->GetModel());
		if (!pStudioModel)
			return;

		matrix3x4_t pBoneToWorldOut[128];
		if (!pPlayer->NewSetupBones(pBoneToWorldOut))
			return;

		mstudiohitboxset_t* pHitboxSet = pStudioModel->GetHitboxSet(0);
		if (!pHitboxSet)
			return;

		auto VectorTransform2 = [](const Vector in1, matrix3x4_t in2, Vector& out)
		{

			out[0] = dot_product(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
			out[1] = dot_product(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
			out[2] = dot_product(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
		};

		for (int i = 0; i < pHitboxSet->numhitboxes; i++)
		{
			mstudiobbox_t* pHitbox = pHitboxSet->GetHitbox(i);
			if (!pHitbox)
				continue;

			Vector vMin, vMax;
			VectorTransform2(pHitbox->bbmin, pBoneToWorldOut[pHitbox->bone], vMin); //nullptr???
			VectorTransform2(pHitbox->bbmax, pBoneToWorldOut[pHitbox->bone], vMax);

			if (pHitbox->m_flRadius != -1.0f)
			{
				g_debug_overlay->AddCapsuleOverlay(vMin, vMax, pHitbox->m_flRadius, 255, 255, 255, 150, duration);

				g_debug_overlay->AddCapsuleOverlay(vMin, vMax, pHitbox->m_flRadius, 255, 255, 255, 150, duration);
			}
		}*/

		if (!pPlayer)
			return;

		matrix3x4_t boneMatrix_actual[MAXSTUDIOBONES];
		if (!pPlayer->new_setup_bones(boneMatrix_actual))
			return;

		studiohdr_t* studioHdr = g_mdl_info->get_studiomodel(pPlayer->get_model());
		if (studioHdr)
		{
			mstudiohitboxset_t* set = studioHdr->get_hitbox_set(pPlayer->hitbox_set());
			if (set)
			{
				for (int i = 0; i < set->numhitboxes; i++)
				{
					mstudiobbox_t* hitbox = set->get_hitbox(i);
					if (hitbox)
					{

						if (hitbox->m_flRadius != -1.0f)
						{
							Vector min_actual = Vector(0, 0, 0);
							Vector max_actual = Vector(0, 0, 0);

							min_actual = math::vector_transform(hitbox->bbmin, boneMatrix_actual[hitbox->bone]);
							max_actual = math::vector_transform(hitbox->bbmax, boneMatrix_actual[hitbox->bone]);

							g_debug_overlay->add_capsule_overlay(min_actual, max_actual, hitbox->m_flRadius, 255, 255, 255, 150, duration);
						}
					}
				}
			}
		}
	}

	void draw_bullet_impacts()
	{
		for (int i = 0; i < globals::bullet_impact::bullet_impacts.size(); i++)
		{
			auto& cur_impact = globals::bullet_impact::bullet_impacts[i];

			g_debug_overlay->add_box_overlay(cur_impact.pos, Vector(-cur_impact.size, -cur_impact.size, -cur_impact.size), Vector(cur_impact.size, cur_impact.size, cur_impact.size), QAngle(0, 0, 0), cur_impact.col.r(), cur_impact.col.g(), cur_impact.col.b(), cur_impact.col.a(), settings::visuals::bullet_impact::box::time_s);

			if (g_global_vars->curtime - cur_impact.delete_time < 0)
				globals::bullet_impact::bullet_impacts.erase(globals::bullet_impact::bullet_impacts.begin() + i);
		}
	}

	void draw_grenades()
	{
		if (!settings::visuals::grenades::enable)
			return;

		for (int i = 0; i < g_entity_list->get_highest_entity_index(); i++)
		{
			auto m_entity = reinterpret_cast<c_base_entity*>(g_entity_list->get_client_entity(i));

			if (m_entity && m_entity != g_local_player)
			{
				draw_granades(m_entity);
			}
		}

		for (int i = 0; i < smoke_info.size(); i++)
		{
			Vector position;

			if (smoke_info[i].time_to_expire - g_global_vars->curtime < 0)
			{
				smoke_info.erase(smoke_info.begin() + i);
				continue;
			}

			if (math::world_to_screen(smoke_info[i].position, position))
			{
				std::string life_time = utils::snprintf("%4.1f", (float)smoke_info[i].time_to_expire - g_global_vars->curtime);

				ImVec2 time_size = ImGui::CalcTextSize(life_time.c_str());


				if (settings::visuals::grenades::smoke_bar)
				{
					float box_w = (float)fabs((position.x - 30) - (position.x + 30));
					auto width = (((box_w * (smoke_info[i].time_to_expire - g_global_vars->curtime)) / 18.f));

					render::draw_box_filled(position.x - 30, position.y + 15, position.x + 30, position.y + 18.f, settings::visuals::grenades::color_bar_smoke_back);
					render::draw_box_filled(position.x - 30, position.y + 15, (position.x - 30) + (int)width, position.y + 18.f, settings::visuals::grenades::color_bar_smoke_main);
					if (settings::visuals::grenades::smoke_timer)
					{
						render::draw_text(
							life_time,
							ImVec2(((position.x - 30) + (int)width) - (time_size.x / 2), position.y + 20.f), 10.f,
							settings::visuals::grenades::color_molotov, true, true, small_font);
					}
				}
				else if (settings::visuals::grenades::smoke_timer)
				{
					render::draw_text(
						life_time,
						ImVec2(position.x, position.y + 10), 10.f,
						settings::visuals::grenades::color_smoke, true, true, small_font);
				}
			}

			if (settings::visuals::grenades::smoke_radius)
			{
				render::draw_circle_3d(smoke_info[i].position, 50, grenade_prediction::get_radius(WEAPON_SMOKEGRENADE), settings::visuals::grenades::color_smoke_radius);
			}
		}

		for (int i = 0; i < molotov_info.size(); i++)
		{
			if (molotov_info[i].time_to_expire - g_global_vars->curtime <= 0)
			{
				molotov_info.erase(molotov_info.begin() + i);
				continue;
			}

			Vector position;
			if (math::world_to_screen(molotov_info[i].position, position))
			{
				std::string life_time = utils::snprintf("%4.1f", (float)molotov_info[i].time_to_expire - g_global_vars->curtime);

				ImVec2 time_size = ImGui::CalcTextSize(life_time.c_str());
				render::draw_text("molotov", position.x, position.y, 12.f, settings::visuals::grenades::color_molotov, true, true, small_font);

				if (settings::visuals::grenades::molotov_bar)
				{
					float box_w = (float)fabs((position.x - 30) - (position.x + 30));
					auto width = (((box_w * (molotov_info[i].time_to_expire - g_global_vars->curtime)) / 7.f));

					render::draw_box_filled(position.x - 30, position.y + 15.f, position.x + 30, position.y + 18.f, settings::visuals::grenades::color_bar_molotov_back);
					render::draw_box_filled(position.x - 30, position.y + 15.f, (position.x - 30) + (int)width, position.y + 18.f, settings::visuals::grenades::color_bar_molotov_main);

					if (settings::visuals::grenades::molotov_timer)
					{
						render::draw_text(
							life_time,
							ImVec2(((position.x - 30) + (int)width) - (time_size.x / 2), position.y + 20.f), 10.f,
							settings::visuals::grenades::color_molotov, true, true, small_font);
					}
				}
				else if (settings::visuals::grenades::molotov_timer)
				{
					render::draw_text(
						life_time,
						ImVec2(position.x, position.y + 10), 10.f,
						settings::visuals::grenades::color_molotov, true, true, small_font);
				}

				if (settings::visuals::grenades::molotov_radius)
				{
					render::draw_circle_3d(molotov_info[i].position, 50, grenade_prediction::get_radius(WEAPON_MOLOTOV), settings::visuals::grenades::color_molotov_radius);
				}

			}
		}
	}

	void draw_aa_lines()
	{
		auto drawAngleLine = [&](const Vector& origin, const Vector& w2sOrigin, const float& angle, const char* text, Color clr) {
			Vector forward;
			math::angle_vectors(QAngle(0.0f, angle, 0.0f), forward);
			float AngleLinesLength = 30.0f;

			Vector w2sReal;
			if (math::world_to_screen(origin + forward * AngleLinesLength, w2sReal)) {
				render::draw_line(w2sOrigin.x, w2sOrigin.y, w2sReal.x, w2sReal.y, Color(255, 255, 255, 255), 1.0f);
				render::draw_box_filled(w2sReal.x - 5.0f, w2sReal.y - 5.0f, w2sReal.x + 5.0f, w2sReal.y + 5.0f, Color(255, 255, 255, 255));
				render::draw_text(text, w2sReal.x, w2sReal.y - 5.0f, 14.0f, clr, true, true);
			}
		};

		if (settings::misc::desync::indicator::lines)
		{
			Vector w2sOrigin;
			if (math::world_to_screen(g_local_player->vec_origin(), w2sOrigin)) {
				drawAngleLine(g_local_player->vec_origin(), w2sOrigin, globals::aa::view_angle, "viewangles", Color(0.937f, 0.713f, 0.094f, 1.0f));
				drawAngleLine(g_local_player->vec_origin(), w2sOrigin, g_local_player->lower_body_yaw_target(), "lby", Color(0.0f, 0.0f, 1.0f, 1.0f));
				drawAngleLine(g_local_player->vec_origin(), w2sOrigin, globals::aa::real_angle, "real", Color(0.0f, 1.0f, 0.0f, 1.0f));
				drawAngleLine(g_local_player->vec_origin(), w2sOrigin, g_local_player->get_abs_angles2().y, "fake", Color(0.0f, 0.0f, 1.0f, 1.0f));
			}
		}
	}

	void draw_aa_arrow()
	{
		if (settings::misc::desync::type == 0 || !settings::misc::desync::indicator::arrow)
			return;

		int screen_w, screen_h;
		g_engine_client->get_screen_size(screen_w, screen_h);

		int centre_x = screen_w / 2;
		int centre_y = screen_h / 2;

		int x_offset = settings::misc::desync::indicator::offset_x;
		int x_size = settings::misc::desync::indicator::size_x;
		int y_size = settings::misc::desync::indicator::size_y;

		std::vector<ImVec2> left{ ImVec2(centre_x + x_offset, centre_y - y_size), ImVec2(centre_x + x_offset, centre_y + y_size), ImVec2(centre_x + x_offset + x_size, centre_y) };
		std::vector<ImVec2> right{ ImVec2(centre_x - x_offset, centre_y - y_size), ImVec2(centre_x - x_offset, centre_y + y_size), ImVec2(centre_x - x_offset - x_size, centre_y) };

		if (settings::misc::desync::bind.enable)
		{
			render::draw_textured_polygon(3, left, settings::misc::desync::indicator::real);
			render::draw_textured_polygon(3, right, settings::misc::desync::indicator::fake);
		}
		if (!settings::misc::desync::bind.enable)
		{
			render::draw_textured_polygon(3, left, settings::misc::desync::indicator::fake);
			render::draw_textured_polygon(3, right, settings::misc::desync::indicator::real);
		}
	}

	void draw_spreed_circle()
	{
		if (!g_local_player->is_alive() || !settings::misc::spreed_circle::enable)
			return;

		auto weapon = g_local_player->active_weapon();

		if (!weapon)
			return;

		float spreed = weapon->get_inaccuracy() * 800;

		int w, h;
		int centre_w, centre_h;

		g_engine_client->get_screen_size(w, h);

		centre_w = w / 2;
		centre_h = h / 2;

		render::draw_circle_filled(centre_w, centre_h, spreed, 40, settings::misc::spreed_circle::color);
	}

	void draw_spectator_list()
	{
		int specs = 0;
		std::vector <spectator_t> spectators;

		if (g_engine_client->is_ingame() && g_engine_client->is_connected())
		{
			if (g_local_player)
			{
				for (int i = 0; i < g_engine_client->get_max_clients(); i++)
				{
					c_base_player* pBaseEntity = c_base_player::get_player_by_index(i);
					if (!pBaseEntity)										     continue;
					if (pBaseEntity->health() > 0)  							 continue;
					if (pBaseEntity == g_local_player)							 continue;
					if (pBaseEntity->is_dormant())								 continue;
					if (g_local_player->is_alive())
					{
						if (pBaseEntity->observer_target() != g_local_player)	 continue;
						if (pBaseEntity->observer_mode() == observer_mode_t::OBS_MODE_ROAMING) continue;
					}
					else
					{
						if (g_local_player->observer_mode() == observer_mode_t::OBS_MODE_ROAMING)
						{
							if (pBaseEntity->observer_mode() != observer_mode_t::OBS_MODE_ROAMING) continue;
						}
						else
						{
							if (pBaseEntity->observer_mode() == observer_mode_t::OBS_MODE_ROAMING) continue;
							/*else
							{*/
								c_base_player* spectated_ent = g_local_player->observer_target();
								if (!spectated_ent) continue;
								if (pBaseEntity->observer_target() != spectated_ent)	 continue;
							//}
						}
					}
					player_info_t pInfo;
					g_engine_client->get_player_info(pBaseEntity->ent_index(), &pInfo);
					if (pInfo.ishltv) continue;
					if (pInfo.fakeplayer) continue;
					if (pInfo.szName == "" || pInfo.szName == " ") continue;

					spectators.push_back(spectator_t{ pInfo.szName , pBaseEntity->observer_mode() });
					//spect += pInfo.szName;
					//spect += "\n";
					//specs++;
				}
			}
		}

		float becup = ImGui::GetStyle().Alpha;
		ImVec4 becup_windowbg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
		ImVec4 becup_text = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImVec4 becup_titlebg = ImGui::GetStyle().Colors[ImGuiCol_TitleBg];
		ImVec4 becup_titlebar = ImGui::GetStyle().Colors[ImGuiCol_SliderGrab];

		bool is_enable = (!spectators.empty() || menu::is_visible()) && settings::visuals::spectator_list;

		ImGui::SetNextWindowPos(settings::windows::Spec_pos, ImGuiCond_Once);

		if (is_enable)
		{
			ImGui::SetNextWindowBgAlpha(settings::windows::Spec_alpha);

			ImGui::GetStyle().Alpha = becup;
			ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = becup_windowbg;
			ImGui::GetStyle().Colors[ImGuiCol_Text] = becup_text;
			ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = becup_titlebg;
			ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = becup_titlebar;
		}
		else
		{
			ImGui::SetNextWindowBgAlpha(0.f);
			ImGui::GetStyle().Alpha = 0.01f;
			ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
			ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.f, 0.f, 0.f, 0.f);
			ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
			ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(0.f, 0.f, 0.f, 0.f);
		}

		ImGui::Begin("spectator list", nullptr, ImVec2(0, 0), settings::windows::Spec_alpha, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_RainbowTitleBar | (menu::is_visible() && is_enable ? 0 : ImGuiWindowFlags_NoMove));
		{
			ImGui::GetCurrentWindow()->Hidden = !is_enable;

			//ImVec2 size = spectatir_t.size();
			//ImGui::SetWindowSize(ImVec2(200, 25 + size.y));

			settings::windows::Spec_pos = ImGui::GetWindowPos();

			ImGui::Columns(2, NULL, false);
			{
				ImGui::Text(std::string("name (" + std::to_string(spectators.size()) + ")").c_str());
				ImGui::SetColumnWidth(-1, 150);
				ImGui::NextColumn();
				ImGui::Text("mode");
			}
			ImGui::Columns(1, NULL, false);
			ImGui::Separator(200.f);

			ImGui::Columns(2, NULL, false);
			{
				for (int i = 0; i < spectators.size(); i++)
				{
					auto& spec = spectators[i];

					ImGui::Text(spec.name.c_str());

					ImGui::NextColumn();

					std::string mode;

					switch (spec.mode)
					{
					case OBS_MODE_IN_EYE:
						mode = "perspective";
						break;
					case OBS_MODE_CHASE:
						mode = "3rd person";
						break;
					case OBS_MODE_ROAMING:
						mode = "no clip";
						break;
					case OBS_MODE_DEATHCAM:
						mode = "deathcam";
						break;
					case OBS_MODE_FREEZECAM:
						mode = "freezecam";
						break;
					case OBS_MODE_FIXED:
						mode = "fixed";
						break;
					}

					ImGui::Text(mode.c_str());

					ImGui::NextColumn();
				}
			}
			ImGui::Columns(1, NULL, false);

			ImGui::End();
		}

		ImGui::GetStyle().Alpha = becup;
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = becup_windowbg;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = becup_text;
		ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = becup_titlebg;
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = becup_titlebar;
	}

	void draw_fov()
	{
		auto pWeapon = g_local_player->active_weapon();
		if (!pWeapon)
			return;

		if (settings::legit_bot::bind_check && !settings::legit_bot::bind.enable)
			return;

		legitbot_settings settings;

		if (settings::legit_bot::settings_category == 0)
			settings = settings::legit_bot::legitbot_items_all;
		else if (settings::legit_bot::settings_category == 1)
			settings = settings::legit_bot::legitbot_items_groups[legit_aimbot::get_group()];
		else if (settings::legit_bot::settings_category == 2)
			settings = settings::legit_bot::legitbot_items[pWeapon->item().item_definition_index()];

		if (settings.enabled)
		{
			float fov;

			if (settings::misc::disable_zoom)
				fov = 90;
			else
				fov = static_cast<float>(g_local_player->get_fov());

			int w, h;
			g_engine_client->get_screen_size(w, h);

			Vector2D screenSize = Vector2D(w, h);
			Vector2D center = screenSize * 0.5f;

			float ratio = screenSize.x / screenSize.y;
			float screenFov = atanf((ratio) * (0.75f) * tan(DEG2RAD(fov * 0.5f)));

			float radiusFOV = tanf(DEG2RAD(legit_aimbot::without_silent())) / tanf(screenFov) * center.x;

			if (settings::visuals::fov::fov_basic)
			{
				if (settings::visuals::fov::fov_basic_background)
				{
					render::draw_circle_filled(center.x, center.y, radiusFOV, 32, Color(settings::visuals::fov::color_fov_basic_background));
				}
				render::draw_circle(center.x, center.y, radiusFOV, 32, Color(settings::visuals::fov::color_fov_basic));
			}

			if (settings.silent_fov > 0 && settings::visuals::fov::fov_silent)
			{
				float silentRadiusFOV = tanf(DEG2RAD(settings.silent_fov)) / tanf(screenFov) * center.x;

				if (settings::visuals::fov::fov_silent_background)
				{
					render::draw_circle_filled(center.x, center.y, silentRadiusFOV, 32, Color(settings::visuals::fov::color_fov_silent_background));
				}
				render::draw_circle(center.x, center.y, silentRadiusFOV, 32, Color(settings::visuals::fov::color_fov_silent));
			}
		}
	}

	void draw_hitmarker()
	{
		if (!g_local_player || !g_local_player->is_alive() || !settings::misc::hitmarker)
			return;

		auto curtime = g_global_vars->realtime;
		float lineSize = 8.f;
		if (flHurtTime >= curtime) 
		{
			int screenSizeX, screenCenterX;
			int screenSizeY, screenCenterY;
			g_engine_client->get_screen_size(screenSizeX, screenSizeY);

			screenCenterX = screenSizeX / 2;
			screenCenterY = screenSizeY / 2;

			Color bg = Color(0, 0, 0, (int)((flHurtTime - curtime) * 255));
			Color white = Color(255, 255, 255, (int)((flHurtTime - curtime) * 255));

			render::draw_line(screenCenterX - lineSize, screenCenterY - lineSize, screenCenterX - (lineSize / 4), screenCenterY - (lineSize / 4), bg, 2.5f);
			render::draw_line(screenCenterX - lineSize, screenCenterY + lineSize, screenCenterX - (lineSize / 4), screenCenterY + (lineSize / 4), bg, 2.5f);
			render::draw_line(screenCenterX + lineSize, screenCenterY + lineSize, screenCenterX + (lineSize / 4), screenCenterY + (lineSize / 4), bg, 2.5f);
			render::draw_line(screenCenterX + lineSize, screenCenterY - lineSize, screenCenterX + (lineSize / 4), screenCenterY - (lineSize / 4), bg, 2.5f);

			render::draw_line(screenCenterX - lineSize, screenCenterY - lineSize, screenCenterX - (lineSize / 4), screenCenterY - (lineSize / 4), white);
			render::draw_line(screenCenterX - lineSize, screenCenterY + lineSize, screenCenterX - (lineSize / 4), screenCenterY + (lineSize / 4), white);
			render::draw_line(screenCenterX + lineSize, screenCenterY + lineSize, screenCenterX + (lineSize / 4), screenCenterY + (lineSize / 4), white);
			render::draw_line(screenCenterX + lineSize, screenCenterY - lineSize, screenCenterX + (lineSize / 4), screenCenterY - (lineSize / 4), white);
		}
	}

	void render()
	{
		draw_hitmarker();
		draw_fov();
		draw_damage_indicator();
		draw_bullet_impacts();
		draw_grenades();
		draw_aa_arrow();
		draw_aa_lines();
		draw_spreed_circle();

		grenade_prediction::paint();

		/*if (g_Options.esp_sounds)
			RenderSounds();*/

		/*if (g_Options.esp_choke_indicator) 
		{
			std::stringstream ss;
			ss << "choked: " << g_client_state->chokedcommands;

			Render::Get().RenderText(ss.str(), ImVec2(10.0f, 450.0f), 14.0f, Color(255, 255, 255, 255), false, true);
		}*/

		/*if (g_Options.esp_hitmarker)
			DrawHitmarker();*/

		for (auto i = 1; i <= g_entity_list->get_highest_entity_index(); ++i)
		{
			auto entity = c_base_entity::get_entity_by_index(i);

			auto entity_2 = c_base_player::get_player_by_index(i);

			if (!entity)
				continue;


			static const float in = 15.f;
			static const float out = 10.f;

			if (i <= g_global_vars->maxClients)
			{
				c_base_player* arr_pl = (c_base_player*)entity;
				if (arr_pl->is_alive())
				{
					if (!arr_pl->is_dormant())
					{
						StoredCurtimePlayerAlpha[arr_pl->ent_index()] = g_global_vars->curtime;
					}
					if (!settings::visuals::ofc::visible_check)
					{
						if (arr_pl->is_dormant() && g_global_vars->curtime - StoredCurtimePlayerAlpha[arr_pl->ent_index()] > 2)
						{
							PlayerAlpha[arr_pl->ent_index()] = utils::lerp(PlayerAlpha[arr_pl->ent_index()], 0.f, 0.2f);
						}
						else if (!arr_pl->is_dormant())
						{
							PlayerAlpha[arr_pl->ent_index()] = utils::lerp(PlayerAlpha[arr_pl->ent_index()], 255.f, 0.1f);
						}
					}
					if (settings::visuals::ofc::visible_check)
					{
						Vector pos;
						bool visible = math::world_to_screen_ofc(get_hitbox_position(arr_pl, 2), pos) && settings::visuals::ofc::visible_check;//g_local_player->CanSeePlayer(arr_pl, HITBOX_CHEST) && Math::WorldToScreen(GetHitboxPosition(arr_pl, 2), pos) && settings::visuals::ofc::visible_check;
						if (visible || arr_pl->is_dormant())
						{
							PlayerAlpha[arr_pl->ent_index()] = utils::lerp(PlayerAlpha[arr_pl->ent_index()], 0.f, 0.2f);
						}

						else if (!visible && !arr_pl->is_dormant())
						{
							PlayerAlpha[arr_pl->ent_index()] = utils::lerp(PlayerAlpha[arr_pl->ent_index()], 255.f, 0.1f);
						}
					}
				}

				if (settings::visuals::radar_ingame && !entity->spotted())
				{
					entity->spotted() = true;
				}

				draw_player_arrow(arr_pl);

				auto cur_player = player();
				if (cur_player.begin((c_base_player*)entity))
				{

					if (!cur_player.ctx.pl->is_dormant())
					{
						StoredCurtimePlayer[cur_player.ctx.pl->ent_index()] = g_global_vars->curtime;
					}
					if (!global_esp_settings.only_visible)
					{
						if (cur_player.ctx.pl->is_dormant() && g_global_vars->curtime - StoredCurtimePlayer[cur_player.ctx.pl->ent_index()] < 2)
						{
							flPlayerAlpha[cur_player.ctx.pl->ent_index()] = (global_esp_settings.dormant.a() - 255) * -1;
						}

						else if (cur_player.ctx.pl->is_dormant() && g_global_vars->curtime - StoredCurtimePlayer[cur_player.ctx.pl->ent_index()] > 2)
						{
							flPlayerAlpha[cur_player.ctx.pl->ent_index()] = utils::lerp(flPlayerAlpha[cur_player.ctx.pl->ent_index()], 255.f, 0.2f);
						}

						else if (!(cur_player.ctx.pl->is_dormant()))
						{
							flPlayerAlpha[cur_player.ctx.pl->ent_index()] = utils::lerp(flPlayerAlpha[cur_player.ctx.pl->ent_index()], 0.f, 0.1f);
						}
					}
					else if (global_esp_settings.only_visible)
					{
						if (!cur_player.ctx.is_visible)
						{
							flPlayerAlpha[cur_player.ctx.pl->ent_index()] = utils::lerp(flPlayerAlpha[cur_player.ctx.pl->ent_index()], 255.f, 0.2f);
						}

						else if (cur_player.ctx.is_visible)
						{
							flPlayerAlpha[cur_player.ctx.pl->ent_index()] = utils::lerp(flPlayerAlpha[cur_player.ctx.pl->ent_index()], 0.f, 0.1f);
						}
					}

					if (global_esp_settings.box)         cur_player.draw_box();
					if (global_esp_settings.skeleton)	     cur_player.draw_skeleton(entity);
					if (global_esp_settings.weapon)      cur_player.draw_weapon();
					if (global_esp_settings.name)        cur_player.draw_name();
					if (global_esp_settings.armor_bar)    cur_player.draw_armour();
					if (global_esp_settings.health_bar)    cur_player.draw_health();
					cur_player.draw_flags();
				}
			}
			else if (settings::visuals::dropped_weapon::enable && entity->is_weapon())
				draw_weapons(static_cast<c_base_combat_weapon*>(entity));
			//else if (g_Options.esp_dropped_weapons && entity->IsDefuseKit())
				//RenderDefuseKit(entity);
			else if (entity->is_planted_c4() && settings::visuals::bomb_timer)
				draw_window_c4(entity);//RenderPlantedC4(entity);
			/*else if (entity->IsLoot() && g_Options.esp_item)
				RenderItemEsp(entity);*/

		}
	}

	void draw_impacts(const c_effect_data& data)
	{
		if (settings::visuals::bullet_impact::box::enable_c && data.origin.IsValid())
		{
			if (data.start.y == g_local_player->vec_origin().y && data.start.x == g_local_player->vec_origin().x) 
				g_debug_overlay->add_box_overlay(data.origin, Vector(-settings::visuals::bullet_impact::box::size_c, -settings::visuals::bullet_impact::box::size_c, -settings::visuals::bullet_impact::box::size_c), Vector(settings::visuals::bullet_impact::box::size_c, settings::visuals::bullet_impact::box::size_c, settings::visuals::bullet_impact::box::size_c), QAngle(0, 0, 0), settings::visuals::bullet_impact::box::color_c.r(), settings::visuals::bullet_impact::box::color_c.g(), settings::visuals::bullet_impact::box::color_c.b(), settings::visuals::bullet_impact::box::color_c.a(), settings::visuals::bullet_impact::box::time_c);
		}

		hooks::impact::o_impact(data);
	}
}