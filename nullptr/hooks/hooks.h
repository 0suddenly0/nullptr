#pragma once

#include <intrin.h>
#include "../features/notify/notify.h"
#include "../features/changers/profile/protobuff_messages.h"
#include "../features/changers/profile/proto_writer.h"
#include "../features/changers/profile/protobuff_messages.h"
#include "../features/changers/profile/protobuffs.h"
#include "../valve_sdk/csgostructs.h"
#include "../helpers/vfunc_hook.h"
#include <d3d9.h>
#pragma intrinsic(_ReturnAddress)  
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../menu/menu.h"
#include "../render/render.h"
#include "../options/globals.h"
#include "../features/visuals/visuals.h"
#include "../features/changers/skin/skins.h"
#include "../features/changers/skin/item_definitions.h"
#include "../features/visuals/glow.h"
#include "../features/visuals/chams.h"
#include "../features/visuals/grenade_prediction.h"
#include "../features/misc/misc.h"
#include "../features/misc/engine_prediction.h"
#include "../features/legit_aimbot/aimbot.h"
#include "../features/legit_aimbot/backtrack.h"
#include "../helpers/input.h"

namespace index
{
    constexpr auto write_usercmd_delta_to_buffer = 24;
    constexpr auto is_playing_demo = 82;
    constexpr auto send_datagram = 46;
    constexpr auto retrieve_message = 2;
    constexpr auto emit_sound = 5;
    constexpr auto end_scene = 42;
    constexpr auto reset = 16;
    constexpr auto paint_traverse = 41;
    constexpr auto create_move = 24;
    constexpr auto play_sound = 82;
    constexpr auto frame_stage_notify = 37;
    constexpr auto draw_model_execute = 21;
    constexpr auto do_post_screen_space_effects = 44;
    constexpr auto cvar_get_bool = 13;
    constexpr auto override_view = 18;
    constexpr auto fire_event = 9;
    constexpr auto lock_cursor = 67;
    constexpr auto list_leaves_in_box = 6;
    constexpr auto suppress_lists = 16;
    constexpr auto scene_end_i = 9;
    constexpr auto read_packet_entities = 64;
}

namespace hooks
{
    extern vfunc_hook netchannel_vhook;
    extern vfunc_hook retrieve_message_vhook;
    extern vfunc_hook direct3d_vhook;
    extern vfunc_hook vguipanel_vhook;
    extern vfunc_hook vguisurf_vhook;
    extern vfunc_hook mdlrender_vhook;
    extern vfunc_hook viewrender_vhook;
    extern vfunc_hook client_vhook;
    extern vfunc_hook render_view_vhook;
    extern vfunc_hook engine_vhook;
    extern vfunc_hook rend_vhook;
    extern vfunc_hook draw_model_overlay_vhook;
    extern vfunc_hook weapon_spread_vhook;
    extern vfunc_hook sv_showimpacts;
    extern vfunc_hook sv_cheats_vhook;
    extern vfunc_hook sound_vhook;
    extern vfunc_hook clientmode_vhook;
    extern vfunc_hook partition_vhook;
    extern vfunc_hook clientstate_vhook;
    extern vfunc_hook bsp_query_vhook;
    extern vfunc_hook gameevents_vhook;
    extern recv_prop_hook* sequence_vhook;


    void initialize();
    void shutdown();

    namespace create_move
    {
        using fn = bool(__thiscall*)(i_client_mode*, float, c_usercmd*);
        bool __stdcall hook(float input_sample_frametime, c_usercmd* cmd);
    }

    namespace end_scene
    {
        using fn = long(__stdcall*)(IDirect3DDevice9*);
        long __stdcall hook(IDirect3DDevice9* device);
    }

    namespace reset
    {
        using fn = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
        long __stdcall hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
    }

    namespace retrieve_message
    {
        profile_protobuff write;

        using fn = EGCResults(__thiscall*)(void*, uint32_t*, void*, uint32_t, uint32_t*);
        EGCResults __stdcall hook(uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
    }

    namespace paint_treverse
    {
        using fn = void(__thiscall*)(i_panel*, vgui::VPANEL, bool, bool);
        void __stdcall hook(vgui::VPANEL panel, bool forceRepaint, bool allowForce);
    }

    namespace fire_event
    {
        using fn = bool(__thiscall*)(i_game_event_manager2*, i_game_event * pEvent);
        bool __stdcall hook(i_game_event* pEvent);
    }

    namespace is_playing_demo
    {
        using fn = bool(__stdcall*)();
        bool __stdcall hook();
    }

    namespace do_post_screen_effects
    {
        using fn = int(__thiscall*)(i_client_mode*, int);
        int  __stdcall hook(int a1);
    }

    namespace draw_model_execute 
    {
        using fn = void(__fastcall*)(void*, int, i_mat_render_context *, const draw_model_state_t &, const model_render_info_t &, matrix3x4_t *);
        void __fastcall  hook(void* _this, int edx, i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
    }

    namespace lock_cursor
    {
        using fn = void(__thiscall*)(i_surface*);
        void __stdcall hook();
    }

    namespace override_view
    {
        using fn = void(__thiscall*)(i_client_mode*, c_view_setup*);
        void __stdcall hook(c_view_setup* vsView);
    }

    namespace send_datagram
    {
        using fn = int(__thiscall*)(void*, void*);
        int __fastcall hook(void* net_channel, void*, void* datagram);
    }

    namespace frame_stage_notify
    {
        using fn = void(__thiscall*)(i_base_clientdll*, frame_stage);
        void __stdcall hook (frame_stage framestage);
    }

    namespace read_packet_entities
    {
        using fn = void(__thiscall*)(void*, void*);
        void __fastcall hook(void* pClientState, void* pEDX, void* pEntInfo);
    }

    namespace emit_sound
    {
        using fn = void(__thiscall*)(void*, i_recipient_filter&, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, int);
        void __stdcall hook(i_recipient_filter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk);
    }

    namespace recv_proxy
    { 
        void hook(const c_recv_proxy_data* pData, void* entity, void* output);    
    }

    namespace suppress_lists
    {
        using fn = bool(__thiscall*)(void*, int, bool);
        void __stdcall hook(int a2, bool a3);
    }

    namespace list_leaves_in_box
    {
        using fn = int(__thiscall*)(void*, const Vector&, const Vector&, unsigned short*, int);
        int __fastcall hook(void* bsp, void* edx, Vector& mins, Vector& maxs, unsigned short* pList, int listMax);
    }

    namespace cvar_hooks
    {
        namespace sv_cheats
        {
            using fn = bool(__thiscall*)(PVOID);
            bool __fastcall hook(PVOID pConVar, void* edx);
        }
        namespace draw_model_overlay
        {
            using fn = bool(__thiscall*)(PVOID);
            bool __fastcall hook(PVOID pConVar, void* edx);
        }
        namespace weapon_spread
        {
            using fn = int(__thiscall*)(void*);
            int __fastcall hook(void* ecx, void* edx);
        }
    }

    namespace impact
    {
        client_effect_callback o_impact;

        void hook();
        void destroy();
    }
}
