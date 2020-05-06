#include "../hooks.h"

namespace hooks
{
	namespace cvar_hooks
	{
		bool __fastcall sv_cheats::hook(PVOID pConVar, void* edx)
		{
			static const auto dwCAM_Think = utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "85 C0 75 30 38 86");

			static auto ofunc = sv_cheats_vhook.get_original<fn>(index::cvar_get_bool);
			if (!ofunc)
				return false;

			if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
				return true;

			return ofunc(pConVar);
		}

		bool __fastcall draw_model_overlay::hook(PVOID pConVar, void* edx)
		{
			static const auto return_to_unknown = utils::pattern_scan(GetModuleHandleW(L"client_panorama.dll"), "85 C0 75 54 8B 0D ? ? ? ?");

			static auto ofunc = draw_model_overlay_vhook.get_original<fn>(index::cvar_get_bool);
			if (!ofunc)
				return false;

			if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(return_to_unknown))
			{
				return true;
			}

			return ofunc(pConVar);
		}

		int __fastcall weapon_spread::hook(void* ecx, void* edx)
		{
			static auto dwRetAddr = utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "85 C0 0F 84 ? ? ? ? E8 ? ? ? ? 99");
			if (_ReturnAddress() != dwRetAddr || !settings::misc::sniper_crosshair || !g_local_player || !g_local_player->active_weapon())
				return weapon_spread_vhook.get_original<fn>(index::cvar_get_bool)(ecx);

			if (g_local_player->is_scoped())
				return weapon_spread_vhook.get_original<fn>(index::cvar_get_bool)(ecx);

			return 2;
		}
	}
}