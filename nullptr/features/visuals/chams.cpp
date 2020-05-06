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

auto global_chams_settings = settings::visuals::chams::chams_items[selected_entity];
auto global_chams_settings_ragdoll = settings::visuals::chams::chams_items_ragdoll[selected_entity];

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

		material_regular       = g_mat_system->find_material("material_regular"      , TEXTURE_GROUP_OTHER);
		material_regular_walls = g_mat_system->find_material("material_regular_walls", TEXTURE_GROUP_OTHER);
		material_glow          = g_mat_system->find_material("material_glow"         , TEXTURE_GROUP_OTHER);
		material_glow_walls    = g_mat_system->find_material("material_glow_walls"   , TEXTURE_GROUP_OTHER);
		material_flat          = g_mat_system->find_material("material_flat"         , TEXTURE_GROUP_OTHER);
		material_flat_walls    = g_mat_system->find_material("material_flat_walls"   , TEXTURE_GROUP_OTHER);
	}

	void shutdown()
	{
		std::remove("csgo\\materials\\material_glow.vmt");
		std::remove("csgo\\materials\\material_glow_walls.vmt");
		std::remove("csgo\\materials\\material_regular.vmt");
		std::remove("csgo\\materials\\material_regular_walls.vmt");
		std::remove("csgo\\materials\\material_flat.vmt");
		std::remove("csgo\\materials\\material_flat_walls.vmt");
	}

	void scene_end_chams()
	{
		for (int i = 0; i < g_entity_list->get_highest_entity_index(); i++)
		{
			auto entity = reinterpret_cast<c_base_player*>(g_entity_list->get_client_entity(i));

			if (entity && entity != g_local_player && settings::visuals::dropped_weapon::enable)
			{
				auto client_class = entity->get_client_class();
				auto model_name = g_mdl_info->get_model_name(entity->get_model());

				if (client_class->class_id == CSensorGrenadeProjectile || client_class->class_id == CAK47 || client_class->class_id == CDEagle || client_class->class_id == CC4 ||
					client_class->class_id >= CWeaponAug && client_class->class_id <= CWeaponXM1014) {
					if (settings::visuals::dropped_weapon::chams)
					{
						override_mat(
							false,
							settings::visuals::dropped_weapon::chams_type,
							settings::visuals::dropped_weapon::chams_color);

						entity->draw_model(1, 255);
					}
				}
				g_mdl_render->forced_material_override(nullptr);
			}
		}
	}
	void override_mat(bool visible_check, int type, const Color& rgba)
	{
		i_material* material = nullptr;

		switch (type)
		{
		case 0: visible_check ? material = material_regular_walls : material = material_regular; break;
		case 1: visible_check ? material = material_flat_walls : material = material_flat; break;
		case 2: visible_check ? material = material_glow_walls : material = material_glow; break;
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
			if (settings::visuals::chams::local_model::hands)
			{
				override_mat(false, settings::visuals::chams::local_model::hands_type, settings::visuals::chams::local_model::hands_color);
				*should_to_draw_original = true;
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

		if (ent_class->class_id == CMolotovProjectile ||      ent_class->class_id == CMolotovGrenade ||
			ent_class->class_id == CDecoyGrenade ||           ent_class->class_id == CDecoyProjectile ||
			ent_class->class_id == CFlashbang ||              ent_class->class_id == CIncendiaryGrenade ||
			ent_class->class_id == CHEGrenade ||              ent_class->class_id == CSmokeGrenade ||
			ent_class->class_id == CSmokeGrenadeProjectile || ent_class->class_id == CSensorGrenadeProjectile ||
			ent_class->class_id == CSensorGrenade ||          ent_class->class_id == CAK47 ||
			ent_class->class_id == CBaseCSGrenade ||          ent_class->class_id == CBaseCSGrenadeProjectile ||
			ent_class->class_id == CDEagle ||                 ent_class->class_id == CC4 ||
			ent_class->class_id >= CWeaponAug &&              ent_class->class_id <= CWeaponXM1014) {
			if (settings::visuals::dropped_weapon::enable && settings::visuals::dropped_weapon::chams)
			{
				override_mat(
					false,
					settings::visuals::dropped_weapon::chams_type,
					settings::visuals::dropped_weapon::chams_color);
				*should_to_draw_original = true;
			}
		}

		if (ent_class->class_id == EClassId::CCSRagdoll)
		{
			if (strstr(model_name, "models/player"))
			{
				const auto enemy = ent->team_num() != g_local_player->team_num();

				if (enemy)
				{
					global_chams_settings_ragdoll = settings::visuals::chams::chams_items_ragdoll[esp_types::enemies];
				}
				else if (!enemy && !(ent == g_local_player))
				{
					global_chams_settings_ragdoll = settings::visuals::chams::chams_items_ragdoll[esp_types::teammates];
				}

				if (!global_chams_settings_ragdoll.enable)
					return;

				if (!global_chams_settings_ragdoll.only_visible)
				{
					if (global_chams_settings_ragdoll.chams_type == 2)
					{
						override_mat(
							true,
							1,
							global_chams_settings_ragdoll.glow_invisible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

						override_mat(
							true,
							global_chams_settings_ragdoll.chams_type,
							global_chams_settings_ragdoll.invisible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

						override_mat(
							false,
							1,
							global_chams_settings_ragdoll.glow_visible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

						override_mat(
							false,
							global_chams_settings_ragdoll.chams_type,
							global_chams_settings_ragdoll.visible);

						*should_to_draw_original = true;
					}
					else
					{
						override_mat(
							true,
							global_chams_settings_ragdoll.chams_type,
							global_chams_settings_ragdoll.invisible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

						override_mat(
							false,
							global_chams_settings_ragdoll.chams_type,
							global_chams_settings_ragdoll.visible);

						*should_to_draw_original = true;
					}
				}
				else
				{
					if (global_chams_settings_ragdoll.chams_type == 2)
					{
						override_mat(
							false,
							1,
							global_chams_settings_ragdoll.glow_visible);

						oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);
					}

					override_mat(
						false,
						global_chams_settings_ragdoll.chams_type,
						global_chams_settings_ragdoll.visible);

					*should_to_draw_original = true;
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

				if (settings::visuals::chams::local_model::real && settings::misc::desync::type != 0)
				{
					Vector fl = g_local_player->get_abs_angles2();

					Vector cham_angle = Vector(fl.x, globals::aa::real_angle, fl.z);

					float color[4] = { 0.8f, 0.8f, 0.8f, 0.2f };

					if (!g_local_player->is_alive())
						return;

					Vector OrigAngle = g_local_player->get_abs_angles2();
					Vector OrigOrigin = g_local_player->get_abs_origin();

					g_local_player->set_abs_original(OrigOrigin);
					g_local_player->set_abs_angles2(cham_angle);

					override_mat(
						false,
						settings::visuals::chams::local_model::real_type,
						settings::visuals::chams::local_model::real_color);

					g_local_player->draw_model(0x1, 255);

					g_local_player->set_abs_angles2(OrigAngle);
					g_local_player->set_abs_original(OrigOrigin);

					g_studio_render->forced_material_override(nullptr);

					*should_to_draw_original = false;
				}

				if (settings::visuals::chams::chams_items[esp_types::local_player].enable)
				{
					override_mat(
						false,
						settings::visuals::chams::chams_items[esp_types::local_player].chams_type,
						settings::visuals::chams::chams_items[esp_types::local_player].visible);

					oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

					*should_to_draw_original = false;
				}
			}
			else
			{
				const auto enemy = ent->team_num() != g_local_player->team_num();

				if (enemy)
				{
					global_chams_settings = settings::visuals::chams::chams_items[esp_types::enemies];
				}
				else if (!enemy && !(ent == g_local_player))
				{
					global_chams_settings = settings::visuals::chams::chams_items[esp_types::teammates];
				}

				if (ent->is_alive())
				{
					if (settings::visuals::chams::backtrack::enable && backtrack::data.count(ent->ent_index()) > 0)
					{
						auto& data = backtrack::data.at(ent->ent_index());
						if (data.size() > 0)
						{
							if (settings::visuals::chams::backtrack::type_draw == 1)
							{
								for (auto& record : data)
								{
									if (!settings::visuals::chams::backtrack::only_visible)
									{
										override_mat(
											true,
											settings::visuals::chams::backtrack::type,
											settings::visuals::chams::backtrack::color_invisible);

										oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, record.boneMatrix);

										override_mat(
											false,
											settings::visuals::chams::backtrack::type,
											settings::visuals::chams::backtrack::color_visible);

										oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, record.boneMatrix);

										g_studio_render->forced_material_override(nullptr);

										*should_to_draw_original = false;
									}
									else
									{
										override_mat(
											false,
											settings::visuals::chams::backtrack::type,
											settings::visuals::chams::backtrack::color_visible);

										oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, record.boneMatrix);

										g_studio_render->forced_material_override(nullptr);

										*should_to_draw_original = false;
									}
								}
							}
							else if (settings::visuals::chams::backtrack::type_draw == 0)
							{
								auto& back = data.back();

								if (!settings::visuals::chams::backtrack::only_visible)
								{
									override_mat(
										true,
										settings::visuals::chams::backtrack::type,
										settings::visuals::chams::backtrack::color_invisible);

									oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, back.boneMatrix);

									override_mat(
										false,
										settings::visuals::chams::backtrack::type,
										settings::visuals::chams::backtrack::color_visible);

								    oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, back.boneMatrix);

									g_studio_render->forced_material_override(nullptr);

									*should_to_draw_original = false;
								}
								else
								{
									override_mat(
										false,
										settings::visuals::chams::backtrack::type,
										settings::visuals::chams::backtrack::color_visible);

									oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, back.boneMatrix);

									*should_to_draw_original = false;
								}
							}
						}
					}


					if (!global_chams_settings.enable)
						return;

					if (!global_chams_settings.only_visible)
					{
						if (global_chams_settings.chams_type == 2)
						{
							override_mat(
								true,
								1,
								global_chams_settings.glow_invisible);

							oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

							override_mat(
								true,
								global_chams_settings.chams_type,
								global_chams_settings.invisible);

							oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

							override_mat(
								false,
								1,
								global_chams_settings.glow_visible);

							oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

							override_mat(
								false,
								global_chams_settings.chams_type,
								global_chams_settings.visible);

							*should_to_draw_original = true;
						}
						else
						{
							override_mat(
								true,
								global_chams_settings.chams_type,
								global_chams_settings.invisible);

							oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);

							override_mat(
								false,
								global_chams_settings.chams_type,
								global_chams_settings.visible);

							*should_to_draw_original = true;
						}
					}
					else
					{
						if (global_chams_settings.chams_type == 2)
						{
							override_mat(
								false,
								1,
								global_chams_settings.glow_visible);

							oDrawModelExecute(g_mdl_render, 0, ctx, state, pInfo, matrix);
						}

						override_mat(
							false,
							global_chams_settings.chams_type,
							global_chams_settings.visible);


						*should_to_draw_original = true;
					}
				}
			}
		}

		if (strstr(model_name, "models/weapons/v_") != nullptr)
		{
			if (settings::visuals::chams::local_model::weapon)
			{
				override_mat(false, settings::visuals::chams::local_model::weapon_type, settings::visuals::chams::local_model::weapon_color);

				*should_to_draw_original = true;
			}
		}

	}
}

