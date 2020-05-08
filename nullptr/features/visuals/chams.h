#pragma once
#include "../../valve_sdk/csgostructs.h"

namespace chams
{
    void init();
    void shutdown();

    void dme_chams(i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& pInfo, matrix3x4_t* matrix, bool* should_to_draw_original);

    void override_mat(bool visible_check, int type, const Color& rgba);

    i_material* material_regular = nullptr;
    i_material* material_regular_walls = nullptr;
    i_material* material_glow = nullptr;
    i_material* material_glow_walls = nullptr;
    i_material* material_flat = nullptr;
    i_material* material_flat_walls = nullptr;
    i_material* material_wireframe = nullptr;
    i_material* material_wireframe_walls = nullptr;
}