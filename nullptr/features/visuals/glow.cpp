#include "glow.h"

int selected_ent;

auto global_glow_item = settings::visuals::glow::glow_items[selected_ent];

namespace glow
{
    void init()
    {

        for (auto i = 0; i < g_glow_obj_manager->m_GlowObjectDefinitions.Count(); i++) 
        {
            auto& glowObject = g_glow_obj_manager->m_GlowObjectDefinitions[i];
            auto entity = reinterpret_cast<c_base_player*>(glowObject.m_pEntity);

            if (glowObject.is_unused())
                continue;

            if (!entity || entity->is_dormant())
                continue;

            auto class_id = entity->get_client_class()->class_id;
            Color color = Color();

            switch (class_id) {
            case 40:
            {
                bool is_visible = g_local_player->can_see_player(entity, HITBOX_CHEST);
                bool is_localplayer = entity == g_local_player;
                bool is_enemy = entity->team_num() != g_local_player->team_num();

                if (is_enemy)
                {
                    global_glow_item = settings::visuals::glow::glow_items[esp_types::enemies];
                }
                if (is_localplayer)
                {
                    global_glow_item = settings::visuals::glow::glow_items[esp_types::local_player];
                }
                if (!is_enemy)
                {
                    global_glow_item = settings::visuals::glow::glow_items[esp_types::teammates];
                }

                if (!global_glow_item.enable)
                    continue;

                if (is_visible)
                {
                    color = global_glow_item.visible;
                    break;
                }
                else if (!is_visible && !global_glow_item.visible_only)
                {
                    color = global_glow_item.in_visible;
                    break;
                }



                /*if (entity->HasC4() && is_enemy && g_Options.glow_c4_carrier) {
                    color = Color(g_Options.color_glow_c4_carrier);
                    break;
                }

                if (!g_Options.glow_players || !entity->IsAlive())
                    continue;

                if (!is_enemy && g_Options.glow_enemies_only)
                    continue;

                color = is_enemy ? Color(g_Options.color_glow_enemy) : Color(g_Options.color_glow_ally);*/

                break;
            }
            /*case ClassId_CChicken:
                if (!g_Options.glow_chickens)
                    continue;
                entity->m_bShouldGlow() = true;
                color = Color(g_Options.color_glow_chickens);
                break;
            case ClassId_CBaseAnimating:
                if (!g_Options.glow_defuse_kits)
                    continue;
                color = Color(g_Options.color_glow_defuse);
                break;
            case ClassId_CPlantedC4:
                if (!g_Options.glow_planted_c4)
                    continue;
                color = Color(g_Options.color_glow_planted_c4);
                break;
            default:
            {
                if (entity->IsWeapon()) {
                    if (!g_Options.glow_weapons)
                        continue;
                    color = Color(g_Options.color_glow_weapons);
                }
            }*/
            }

            glowObject.m_flRed = color.r() / 255.0f;
            glowObject.m_flGreen = color.g() / 255.0f;
            glowObject.m_flBlue = color.b() / 255.0f;
            glowObject.m_flAlpha = color.a() / 255.0f;
            glowObject.m_bRenderWhenOccluded = true;
            glowObject.m_bRenderWhenUnoccluded = false;
        }
    }

    void shutdown()
    {
        for (auto i = 0; i < g_glow_obj_manager->m_GlowObjectDefinitions.Count(); i++) {
            auto& glowObject = g_glow_obj_manager->m_GlowObjectDefinitions[i];
            auto entity = reinterpret_cast<c_base_player*>(glowObject.m_pEntity);

            if (glowObject.is_unused())
                continue;

            if (!entity || entity->is_dormant())
                continue;

            glowObject.m_flAlpha = 0.0f;
        }
    }
}
