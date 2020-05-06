#include "sdk.h"
#include "csgostructs.h"

#include "../Helpers/Utils.h"

i_weapon_system*                 g_weapon_system     = nullptr;
c_client_effect_registration*    g_effects_head      = nullptr;
i_vengine_client*                g_engine_client     = nullptr;
i_base_clientdll*                g_chl_client        = nullptr;
i_client_entity_list*            g_entity_list       = nullptr;
c_global_varsbase*               g_global_vars       = nullptr;
i_engine_trace*                  g_engine_trace      = nullptr;
i_cvar*                          g_cvar              = nullptr;
i_panel*                         g_vgui_panel        = nullptr;
i_client_mode*                   g_client_mode       = nullptr;
i_vdebug_overlay*                g_debug_overlay     = nullptr;
i_surface*                       g_vgui_surface      = nullptr;
c_input*                         g_input             = nullptr;
i_vmodelinfo_client*             g_mdl_info          = nullptr;
i_view_render_beams*             g_beam              = nullptr;
i_vmodel_render*                 g_mdl_render        = nullptr;
i_vrender_view*                  g_render_view       = nullptr;
i_material_system*               g_mat_system        = nullptr;
i_game_event_manager2*           g_game_events       = nullptr;
i_move_helper*                   g_move_helper       = nullptr;
i_mdl_cache*                     g_mdl_cache         = nullptr;
i_prediction*                    g_prediction        = nullptr;
c_gamemovement*                  g_game_movement     = nullptr;
i_enginesound*                   g_engine_sound      = nullptr;
c_glow_object_manager*           g_glow_obj_manager  = nullptr;
i_view_render*                   g_view_render       = nullptr;
IDirect3DDevice9*                g_d3ddevice9        = nullptr;
c_clientstate*                   g_client_state      = nullptr;
c_cs_player_resource*            g_player_resource   = nullptr;
i_physics_surface_props*         g_phys_surface      = nullptr;
i_inputsystem*                   g_input_system      = nullptr;
i_memalloc*				     	 g_mem_alloc		 = nullptr;
uintptr_t*						 g_spatial_partition = nullptr;
i_studiorender*			         g_studio_render     = nullptr;
c_localplayer                    g_local_player;
i_localize*                      g_localize          = nullptr;
i_base_file_system*              g_base_file_system  = nullptr;
i_steam_game_coordinator*        g_game_coordinator  = nullptr;
i_steam_user*                    g_steam_user        = nullptr;
i_steam_http*                    g_steam_http        = nullptr;
i_steam_client*                  g_steam_client      = nullptr;
i_steam_friends*                 g_steam_friends     = nullptr;

int( __cdecl* RandomInt )( int min, int max );
void( __cdecl* RandomSeed )( uint32_t seed );
float( __cdecl* RandomFloat )( float min, float max );


namespace interfaces
{
    create_interface_fn get_module_factory(HMODULE module)
    {
        return reinterpret_cast<create_interface_fn>(GetProcAddress(module, "CreateInterface"));
    }

    class InterfaceReg
    {
    private:

        using instantiate_interface_fn = void* (*)();

    public:

        instantiate_interface_fn m_CreateFn;
        const char* m_pName;

        InterfaceReg* m_pNext;
    };

    template<typename T>
    T* get_interface(create_interface_fn f, const char* szInterfaceVersion)
    {
        auto result = reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));

        if(!result) {
            throw std::runtime_error(std::string("[get_interface] Failed to GetOffset interface: ") + szInterfaceVersion);
        }

        return result;
    }

    template<typename T>
    T* get_interface_w_index(const char* modName, const char* ifaceName, bool exact = false)
    {
        T* iface = nullptr;
        InterfaceReg* ifaceRegList;
        int partMatchLen = strlen(ifaceName);

        DWORD ifaceFn = reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleA(modName), "CreateInterface"));

        if (!ifaceFn)
            return nullptr;

        unsigned int jmpStart = (unsigned int)(ifaceFn)+4;
        unsigned int jmpTarget = jmpStart + *(unsigned int*)(jmpStart + 1) + 5;

        ifaceRegList = **reinterpret_cast<InterfaceReg***>(jmpTarget + 6);

        for (InterfaceReg* cur = ifaceRegList; cur; cur = cur->m_pNext)
        {
            if (exact == true)
            {
                if (strcmp(cur->m_pName, ifaceName) == 0)
                    iface = reinterpret_cast<T*>(cur->m_CreateFn());
            }
            else
            {
                if (!strncmp(cur->m_pName, ifaceName, partMatchLen) && std::atoi(cur->m_pName + partMatchLen) > 0)
                    iface = reinterpret_cast<T*>(cur->m_CreateFn());
            }
        }
        return iface;
    }

	template <typename T>
	T get_steam_interface(const char* version)
	{
		static const auto steam_api = GetModuleHandleA("steam_api.dll");
		return reinterpret_cast<T(__cdecl*)(void)>(GetProcAddress(steam_api, version))();
	}

    void initialize()
    {
        auto client = GetModuleHandleW(L"client_panorama.dll");
		auto engine = GetModuleHandleW(L"engine.dll");
		auto dx9api = GetModuleHandleW(L"shaderapidx9.dll");
		auto stdlib = GetModuleHandleA( "vstdlib.dll" );

        auto engineFactory    = get_module_factory(GetModuleHandleW(L"engine.dll"));
        auto clientFactory    = get_module_factory(GetModuleHandleW(L"client_panorama.dll"));
        auto vguiFactory      = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll"));
        auto vgui2Factory     = get_module_factory(GetModuleHandleW(L"vgui2.dll"));
        auto matSysFactory    = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
        auto dataCacheFactory = get_module_factory(GetModuleHandleW(L"datacache.dll"));
        auto vphysicsFactory  = get_module_factory(GetModuleHandleW(L"vphysics.dll"));
        auto inputSysFactory  = get_module_factory(GetModuleHandleW(L"inputsystem.dll"));
		auto localizeFactory  = get_module_factory(GetModuleHandleW(L"localize.dll"));
		auto studioFactory    = get_module_factory(GetModuleHandleW(L"studiorender.dll"));
        auto filesysFactory   = get_module_factory(GetModuleHandleW(L"filesystem_stdio.dll"));
		auto valveStdFactory  = get_module_factory(stdlib);

		const auto _steam_user = get_steam_interface<HSteamUser>("SteamAPI_GetHSteamUser");
		const auto _steam_pipe = get_steam_interface<HSteamPipe>("SteamAPI_GetHSteamPipe");

        g_steam_client              = get_steam_interface<i_steam_client*>(                          "SteamClient");
        g_base_file_system         = get_interface<i_base_file_system>             (filesysFactory  , "VBaseFileSystem011");
        g_chl_client               = get_interface<i_base_clientdll>             (clientFactory   , "VClient018");
        g_entity_list               = get_interface<i_client_entity_list>         (clientFactory   , "VClientEntityList003");
        g_prediction               = get_interface<i_prediction>                 (clientFactory   , "VClientPrediction001");
        g_game_movement             = get_interface<c_gamemovement>               (clientFactory   , "GameMovement001");
        g_mdl_cache                = get_interface<i_mdl_cache>                  (dataCacheFactory, "MDLCache004");
        g_engine_client             = get_interface<i_vengine_client>             (engineFactory   , "VEngineClient014");
        g_mdl_info                 = get_interface<i_vmodelinfo_client>          (engineFactory   , "VModelInfoClient004");
        g_mdl_render               = get_interface<i_vmodel_render>               (engineFactory   , "VEngineModel016");
        g_render_view               = get_interface<i_vrender_view>                (engineFactory   , "VEngineRenderView014");
        g_engine_trace              = get_interface<i_engine_trace>               (engineFactory   , "EngineTraceClient004");
        g_debug_overlay             = get_interface_w_index<i_vdebug_overlay>     ("engine.dll"    , "VDebugOverlay");
        g_game_events               = get_interface_w_index<i_game_event_manager2>  ("engine.dll"    , "GAMEEVENTSMANAGER002", true);
        g_engine_sound              = get_interface<i_enginesound>                (engineFactory   , "IEngineSoundClient003");
        g_mat_system               = get_interface<i_material_system>             (matSysFactory   , "VMaterialSystem080");
        g_cvar                     = get_interface<i_cvar>                       (valveStdFactory , "VEngineCvar007");
        g_vgui_panel                = get_interface<i_panel>                      (vgui2Factory    , "VGUI_Panel009");
        g_vgui_surface              = get_interface<i_surface>                    (vguiFactory     , "VGUI_Surface031");
        g_phys_surface              = get_interface<i_physics_surface_props>      (vphysicsFactory , "VPhysicsSurfaceProps001");
        g_input_system              = get_interface<i_inputsystem>                (inputSysFactory , "InputSystemVersion001");
		g_spatial_partition		   = get_interface<uintptr_t> 					 (engineFactory   , "SpatialPartition001" );
		g_localize				   = get_interface<i_localize>	    			 (localizeFactory , "Localize_001");
		g_studio_render             = get_interface<i_studiorender>               (studioFactory   , "VStudioRender026");

        c_cs_player_resource** g_player_resource_temp = nullptr;

        g_steam_user                = g_steam_client->get_i_steam_user(                                                   _steam_user, _steam_user, "SteamUser019");
        g_game_coordinator          = static_cast<i_steam_game_coordinator*>(g_steam_client->get_i_steamgeneric_interface(_steam_user, _steam_pipe, "SteamGameCoordinator001"));
        g_steam_friends             = g_steam_client->get_i_steam_friends(                                                _steam_user, _steam_pipe, "SteamFriends015");
        g_steam_http                = g_steam_client->get_i_steam_http(                                                   _steam_user, _steam_pipe, "STEAMHTTP_INTERFACE_VERSION002");
        g_beam                     = *(i_view_render_beams**)(utils::pattern_scan(client                        , "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9") + 1);
		g_mem_alloc                 = *(i_memalloc**)GetProcAddress( GetModuleHandleA( "tier0.dll" )           , "g_pMemAlloc" );
        g_global_vars               = **(c_global_varsbase***)(utils::pattern_scan(client                      , "A1 ? ? ? ? 5E 8B 40 10") + 1);
        g_input                    = *(c_input**)(utils::pattern_scan(client                                  , "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 1);
        g_move_helper              = **(i_move_helper***)(utils::pattern_scan(client                          , "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01") + 2);
        g_glow_obj_manager         = *(c_glow_object_manager**)(utils::pattern_scan(client                    , "0F 11 05 ? ? ? ? 83 C8 01") + 3);
        g_view_render               = *(i_view_render**)(utils::pattern_scan(client                             , "A1 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? FF 10") + 1);
        g_player_resource_temp = *reinterpret_cast<c_cs_player_resource***>(utils::pattern_scan(client, "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7") + 2);
        g_d3ddevice9               = **(IDirect3DDevice9***)(utils::pattern_scan(dx9api                       , "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
        g_client_state              = **(c_clientstate***)(utils::pattern_scan(engine                          , "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);
		g_gamerules                = *(c_game_rules**)(utils::pattern_scan( client                            , "8B 0D ?? ?? ?? ?? 85 C0 74 0A 8B 01 FF 50 78 83 C0 54" ) + 2 );
        g_local_player              = *(c_localplayer*)(utils::pattern_scan(client                             , "8B 0D ? ? ? ? 83 FF FF 74 07") + 2);
        g_effects_head             = **(c_client_effect_registration***)(utils::pattern_scan(client,          "8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? BB FF FF ? ? 8B 0E") + 2);
        g_weapon_system            = *(i_weapon_system**)(utils::pattern_scan(client,                         "8B 35 ? ? ? ? FF 10 0F B7 C0") + 2);

        g_client_mode = **(i_client_mode***)((*(DWORD**)g_chl_client)[10] + 0x5);


		RandomInt = reinterpret_cast< decltype( RandomInt ) >( GetProcAddress( stdlib, "RandomInt" ) );
		RandomSeed = reinterpret_cast< decltype( RandomSeed ) >( GetProcAddress( stdlib, "RandomSeed" ) );
		RandomFloat = reinterpret_cast< decltype( RandomFloat ) >( GetProcAddress( stdlib, "RandomFloat" ) );

        g_player_resource = *g_player_resource_temp;
    }

    void dump()
    {
        // Ugly macros ugh
        #define STRINGIFY_IMPL(s) #s
        #define STRINGIFY(s)      STRINGIFY_IMPL(s)
        #define PRINT_INTERFACE(name) utils::console_print("%-20s: %p\n", STRINGIFY(name), name)

        PRINT_INTERFACE(g_chl_client      );
        PRINT_INTERFACE(g_entity_list     );
        PRINT_INTERFACE(g_prediction      );
        PRINT_INTERFACE(g_game_movement   );
        PRINT_INTERFACE(g_mdl_cache       );
        PRINT_INTERFACE(g_engine_client   );
        PRINT_INTERFACE(g_mdl_info        );
        PRINT_INTERFACE(g_mdl_render      );
        PRINT_INTERFACE(g_render_view     );
        PRINT_INTERFACE(g_engine_trace    );
        PRINT_INTERFACE(g_debug_overlay   );
        PRINT_INTERFACE(g_game_events     );
        PRINT_INTERFACE(g_engine_sound    );
        PRINT_INTERFACE(g_mat_system      );
        PRINT_INTERFACE(g_cvar            );
        PRINT_INTERFACE(g_vgui_panel      );
        PRINT_INTERFACE(g_vgui_surface    );
        PRINT_INTERFACE(g_phys_surface    );
        PRINT_INTERFACE(g_input_system    );
		PRINT_INTERFACE(g_player_resource );
		PRINT_INTERFACE(g_game_coordinator);
        PRINT_INTERFACE(g_base_file_system);
        PRINT_INTERFACE(g_effects_head    );
        PRINT_INTERFACE(g_weapon_system   );
    }
}
