#include "../hooks.h"

namespace hooks
{
	void __fastcall draw_model_execute::hook(void* _this, int edx, i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_vhook.get_original<fn>(index::draw_model_execute);

		bool shadow_depth = (pInfo.flags & STUDIO_SHADOWDEPTHTEXTURE) != 0;

		if (!g_studio_render->is_forced_material_override() && !shadow_depth)
		{
			bool should_to_draw_original = true;
			chams::dme_chams(ctx, state, pInfo, pCustomBoneToWorld, &should_to_draw_original);

     		if (g_local_player)
			{
				if (g_local_player->is_alive())
				{
					i_material* xblur_mat = g_mat_system->find_material("dev/blurfilterx_nohdr", TEXTURE_GROUP_OTHER, true);
					i_material* yblur_mat = g_mat_system->find_material("dev/blurfiltery_nohdr", TEXTURE_GROUP_OTHER, true);
					i_material* scope = g_mat_system->find_material("dev/scope_bluroverlay", TEXTURE_GROUP_OTHER, true);

					if (settings::misc::disable_zoom || settings::misc::disable_zoom_border)
					{
						xblur_mat->set_material_var_flag(MATERIAL_VAR_NO_DRAW, true);
						yblur_mat->set_material_var_flag(MATERIAL_VAR_NO_DRAW, true);
						scope->set_material_var_flag(MATERIAL_VAR_NO_DRAW, true);
					}
					else
					{
						xblur_mat->set_material_var_flag(MATERIAL_VAR_NO_DRAW, false);
						yblur_mat->set_material_var_flag(MATERIAL_VAR_NO_DRAW, false);
						scope->set_material_var_flag(MATERIAL_VAR_NO_DRAW, false);
					}
				}
			}

			if (should_to_draw_original)
				ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

			g_studio_render->forced_material_override(nullptr);
		}
		else
			return ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
	}
}