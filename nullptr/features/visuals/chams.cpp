#include <fstream>
#include "chams.h"
#include "../../hooks/hooks.h"
#include "../../options/options.h"
#include "../../helpers/input.h"
#include "../notify/notify.h"
#include "../../render/render.h"
#include "../../helpers/math.h"
#include "../legit_aimbot/backtrack.h"

int selected_entity;

auto global_chams_settings = settings::visuals::chams::player_settings[selected_entity];
auto global_chams_settings_ragdoll = settings::visuals::chams::ragdoll_settings[selected_entity];

namespace chams
{
	void init()
	{
		std::ofstream("csgo\\materials\\material_regular.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
		std::ofstream("csgo\\materials\\material_regular_walls.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

		std::ofstream("csgo\\materials\\material_wireframe.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
		std::ofstream("csgo\\materials\\material_wireframe_walls.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

		std::ofstream("csgo\\materials\\material_glow.vmt") << R"#("VertexLitGeneric" 
{
 "$additive"               "1"
 "$envmap"                 "models/effects/cube_white"
 "$envmaptint"             "[1 1 1]"
 "$envmapfresnel"          "1"
 "$envmapfresnelminmaxexp" "[0 1 2]"
 "$ignorez"                "0"
}
)#";
		std::ofstream("csgo\\materials\\material_glow_walls.vmt") << R"#("VertexLitGeneric" 
{
 "$additive"               "1"
 "$envmap"                 "models/effects/cube_white"
 "$envmaptint"             "[1 1 1]"
 "$envmapfresnel"          "1"
 "$envmapfresnelminmaxexp" "[0 1 2]"
 "$ignorez"                "1"
}
)#";

		std::ofstream("csgo\\materials\\material_flat.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
		std::ofstream("csgo\\materials\\material_flat_walls.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

		material_wireframe       = g_mat_system->find_material("material_wireframe"      , TEXTURE_GROUP_OTHER);
		material_wireframe_walls = g_mat_system->find_material("material_wireframe_walls", TEXTURE_GROUP_OTHER);
		material_regular         = g_mat_system->find_material("material_regular"        , TEXTURE_GROUP_OTHER);
		material_regular_walls   = g_mat_system->find_material("material_regular_walls"  , TEXTURE_GROUP_OTHER);
		material_glow            = g_mat_system->find_material("material_glow"           , TEXTURE_GROUP_OTHER);
		material_glow_walls      = g_mat_system->find_material("material_glow_walls"     , TEXTURE_GROUP_OTHER);
		material_flat            = g_mat_system->find_material("material_flat"           , TEXTURE_GROUP_OTHER);
		material_flat_walls      = g_mat_system->find_material("material_flat_walls"     , TEXTURE_GROUP_OTHER);
	}

	void shutdown()
	{
		std::remove("csgo\\materials\\material_wireframe.vmt");
		std::remove("csgo\\materials\\material_wireframe_walls.vmt");
		std::remove("csgo\\materials\\material_glow.vmt");
		std::remove("csgo\\materials\\material_glow_walls.vmt");
		std::remove("csgo\\materials\\material_regular.vmt");
		std::remove("csgo\\materials\\material_regular_walls.vmt");
		std::remove("csgo\\materials\\material_flat.vmt");
		std::remove("csgo\\materials\\material_flat_walls.vmt");
	}

	void override_mat(bool visible_check, int type, const Color& rgba)
	{
		i_material* material = nullptr;

		switch (type)
		{
		case 0: visible_check ? material = material_regular_walls : material = material_regular; break;
		case 1: visible_check ? material = material_flat_walls : material = material_flat; break;
		case 2: visible_check ? material = material_glow_walls : material = material_glow; break;
		case 3: visible_check ? material = material_wireframe_walls : material = material_wireframe; material->set_material_var_flag(MATERIAL_VAR_WIREFRAME, true); break;
		}

		if (type == 2)
		{
			bool found = false;
			auto pVar = material->find_var("$envmaptint", &found);
			if (found && pVar)
			{
				(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, rgba.r() / 255.f, rgba.g() / 255.f, rgba.b() / 255.f);
			}
		}

		material->alpha_modulate(
			rgba.a() / 255.f);

		material->color_modulate(
			rgba.r() / 255.0f,
			rgba.g() / 255.0f,
			rgba.b() / 255.0f);

		material->increment_reference_count();

		g_studio_render->forced_material_override(material);
	}


	void dme_chams(i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& pInfo, matrix3x4_t* matrix, bool* should_to_draw_original)
	{
		static auto oDrawModelExecute = hooks::mdlrender_vhook.get_original<hooks::draw_model_execute::fn>(index::draw_model_execute);

		const char* model_name = pInfo.pModel->szName;

		if (!model_name || !g_local_player)
			return;

		auto ent = (c_base_player*)g_entity_list->get_client_entity(pInfo.entity_index);

		if (std::strstr(model_name, "arms") || std::strstr(model_name, "sleeve"))
		{
			auto cur_settings = settings::visuals::chams::hands_settings;

			//set bot/top layer !visible
			if (cur_settings.bot.enable)
			{
				override_mat(
					false,
					cur_settings.bot.chams_type,
					cur_settings.bot.visible);

				oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

				*should_to_draw_original = false;
			}

			if (cur_settings.top.enable)
			{
				override_mat(
					false,
					cur_settings.top.chams_type,
					cur_settings.top.visible);

				oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

				*should_to_draw_original = false;
			}
		}

		if (!ent)
		{
			*should_to_draw_original = true;
			return;
		}


		client_class* ent_class = ent->get_client_class();

		if (!ent_class)
			return; 

		if (ent_class->class_id == CMolotovProjectile || ent_class->class_id == CMolotovGrenade ||
			ent_class->class_id == CDecoyGrenade || ent_class->class_id == CDecoyProjectile ||
			ent_class->class_id == CFlashbang || ent_class->class_id == CIncendiaryGrenade ||
			ent_class->class_id == CHEGrenade || ent_class->class_id == CSmokeGrenade ||
			ent_class->class_id == CSmokeGrenadeProjectile || ent_class->class_id == CSensorGrenadeProjectile ||
			ent_class->class_id == CSensorGrenade || ent_class->class_id == CAK47 ||
			ent_class->class_id == CBaseCSGrenade || ent_class->class_id == CBaseCSGrenadeProjectile ||
			ent_class->class_id == CDEagle || ent_class->class_id == CC4 ||
			ent_class->class_id >= CWeaponAug && ent_class->class_id <= CWeaponXM1014)
		{

			auto cur_settings = settings::visuals::chams::weapon_dropped_settings;

			//set bot/top layer visible
			if (cur_settings.bot.enable)
			{
				override_mat(
					false,
					cur_settings.bot.chams_type,
					cur_settings.bot.visible);

				oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

				*should_to_draw_original = false;
			}

			if (cur_settings.top.enable)
			{
				override_mat(
					false,
					cur_settings.top.chams_type,
					cur_settings.top.visible);

				oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

				*should_to_draw_original = false;
			}
		}

		if (ent_class->class_id == EClassId::CCSRagdoll)
		{
			if (strstr(model_name, "models/player"))
			{
				const auto enemy = ent->team_num() != g_local_player->team_num();

				if (enemy)
				{
					global_chams_settings_ragdoll = settings::visuals::chams::ragdoll_settings[esp_types::enemies];
				}
				else if (!enemy && !(ent == g_local_player))
				{
					global_chams_settings_ragdoll = settings::visuals::chams::ragdoll_settings[esp_types::teammates];
				}

				//set bot/top layer !visible
				if (global_chams_settings_ragdoll.bot.enable && !global_chams_settings_ragdoll.bot.only_visible)
				{
					override_mat(
						true,
						global_chams_settings_ragdoll.bot.chams_type,
						global_chams_settings_ragdoll.bot.invisible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

					*should_to_draw_original = false;
				}

				if (global_chams_settings_ragdoll.top.enable && !global_chams_settings_ragdoll.top.only_visible)
				{
					override_mat(
						true,
						global_chams_settings_ragdoll.top.chams_type,
						global_chams_settings_ragdoll.top.invisible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

					*should_to_draw_original = false;
				}

				//set bot/top layer visible
				if (global_chams_settings_ragdoll.bot.enable)
				{
					override_mat(
						false,
						global_chams_settings_ragdoll.bot.chams_type,
						global_chams_settings_ragdoll.bot.visible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

					*should_to_draw_original = false;
				}

				if (global_chams_settings_ragdoll.top.enable)
				{
					override_mat(
						false,
						global_chams_settings_ragdoll.top.chams_type,
						global_chams_settings_ragdoll.top.visible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);
					
					*should_to_draw_original = false;
				}
			}
		}

		if (ent_class->class_id == EClassId::CCSPlayer)
		{
			if (ent == g_local_player)
			{
				if (g_local_player->is_scoped())
				{
					g_render_view->set_blend(0.4f);
				}

				auto cur_player_settigns = settings::visuals::chams::player_settings[esp_types::local_player];

				//set bot/top layer !visible
				if (cur_player_settigns.bot.enable && !cur_player_settigns.bot.only_visible)
				{
					override_mat(
						true,
						cur_player_settigns.bot.chams_type,
						cur_player_settigns.bot.invisible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

					*should_to_draw_original = false;
				}

				if (cur_player_settigns.top.enable && !cur_player_settigns.top.only_visible)
				{
					override_mat(
						true,
						cur_player_settigns.top.chams_type,
						cur_player_settigns.top.invisible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

					*should_to_draw_original = false;
				}

				//set bot/top layer visible
				if (cur_player_settigns.bot.enable)
				{
					override_mat(
						false,
						cur_player_settigns.bot.chams_type,
						cur_player_settigns.bot.visible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

					*should_to_draw_original = false;
				}

				if (cur_player_settigns.top.enable)
				{
					override_mat(
						false,
						cur_player_settigns.top.chams_type,
						cur_player_settigns.top.visible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

					*should_to_draw_original = false;
				}
			}
			else
			{
				const auto enemy = ent->team_num() != g_local_player->team_num();

				if (enemy)
				{
					global_chams_settings = settings::visuals::chams::player_settings[esp_types::enemies];
				}
				else if (!enemy && !(ent == g_local_player))
				{
					global_chams_settings = settings::visuals::chams::player_settings[esp_types::teammates];
				}

				if (ent->is_alive())
				{
					if (backtrack::data.count(ent->ent_index()) > 0)
					{
						auto& cur_settings_backtrack = settings::visuals::chams::backtrack_settings;

						auto& data = backtrack::data.at(ent->ent_index());
						if (data.size() > 0)
						{
							if (settings::visuals::chams::tick_draw == 1)
							{
								for (auto& record : data)
								{
									//set bot/top layer !visible
									if (cur_settings_backtrack.bot.enable && !cur_settings_backtrack.bot.only_visible)
									{
										override_mat(
											true,
											cur_settings_backtrack.bot.chams_type,
											cur_settings_backtrack.bot.invisible);

										oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, record.boneMatrix);

										*should_to_draw_original = true;
									}

									if (cur_settings_backtrack.top.enable && !cur_settings_backtrack.top.only_visible)
									{
										override_mat(
											true,
											cur_settings_backtrack.top.chams_type,
											cur_settings_backtrack.top.invisible);

										oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, record.boneMatrix);

										*should_to_draw_original = true;
									}

									//set bot/top layer visible
									if (cur_settings_backtrack.bot.enable)
									{
										override_mat(
											false,
											cur_settings_backtrack.bot.chams_type,
											cur_settings_backtrack.bot.visible);

										oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, record.boneMatrix);

										*should_to_draw_original = true;
									}

									if (cur_settings_backtrack.top.enable)
									{
										override_mat(
											false,
											cur_settings_backtrack.top.chams_type,
											cur_settings_backtrack.top.visible);

										oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, record.boneMatrix);

										*should_to_draw_original = true;
									}

									g_studio_render->forced_material_override(nullptr);
								}
							}
							else if (settings::visuals::chams::tick_draw == 0)
							{
								auto& back = data.back();

								if (cur_settings_backtrack.bot.enable && !cur_settings_backtrack.bot.only_visible)
								{
									override_mat(
										true,
										cur_settings_backtrack.bot.chams_type,
										cur_settings_backtrack.bot.invisible);

									oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, back.boneMatrix);
								}

								if (cur_settings_backtrack.top.enable && !cur_settings_backtrack.top.only_visible)
								{
									override_mat(
										true,
										cur_settings_backtrack.top.chams_type,
										cur_settings_backtrack.top.invisible);

									oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, back.boneMatrix);
								}

								//set bot/top layer visible
								if (cur_settings_backtrack.bot.enable)
								{
									override_mat(
										false,
										cur_settings_backtrack.bot.chams_type,
										cur_settings_backtrack.bot.visible);

									oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, back.boneMatrix);
								}

								if (cur_settings_backtrack.top.enable)
								{
									override_mat(
										false,
										cur_settings_backtrack.top.chams_type,
										cur_settings_backtrack.top.visible);

									oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, back.boneMatrix);
								}

								g_studio_render->forced_material_override(nullptr);
							}
						}
					}

					//set bot/top layer !visible
					if (global_chams_settings.bot.enable && !global_chams_settings.bot.only_visible)
					{
						override_mat(
							true,
							global_chams_settings.bot.chams_type,
							global_chams_settings.bot.invisible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

						*should_to_draw_original = false;
					}

					if (global_chams_settings.top.enable && !global_chams_settings.top.only_visible)
					{
						override_mat(
							true,
							global_chams_settings.top.chams_type,
							global_chams_settings.top.invisible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

						*should_to_draw_original = false;
					}

					//set bot/top layer visible
					if (global_chams_settings.bot.enable)
					{
						override_mat(
							false,
							global_chams_settings.bot.chams_type,
							global_chams_settings.bot.visible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

						*should_to_draw_original = false;
					}

					if (global_chams_settings.top.enable)
					{
						override_mat(
							false,
							global_chams_settings.top.chams_type,
							global_chams_settings.top.visible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

						*should_to_draw_original = false;
					}

				}
			}
		}

		if (strstr(model_name, "models/weapons/v_") != nullptr)
		{
			auto& cur_weapon_settings = settings::visuals::chams::weapon_settings;

			//set bot/top layer visible
			if (cur_weapon_settings.bot.enable)
			{
				override_mat(
					false,
					cur_weapon_settings.bot.chams_type,
					cur_weapon_settings.bot.visible);

				oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

				*should_to_draw_original = false;
			}

			if (cur_weapon_settings.top.enable)
			{
				override_mat(
					false,
					cur_weapon_settings.top.chams_type,
					cur_weapon_settings.top.visible);

				oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

				*should_to_draw_original = false;
			}
		}
	}
}

