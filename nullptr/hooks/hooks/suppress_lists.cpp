#include "../hooks.h"

namespace hooks
{
	void __stdcall suppress_lists::hook(int a2, bool a3) {
		static auto ofunc = partition_vhook.get_original<fn>(index::suppress_lists);

		static auto OnRenderStart_Return = utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "FF 50 40 8B 1D ? ? ? ?") + 0x3;
		static auto FrameNetUpdateEnd_Return = utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "5F 5E 5D C2 04 00 83 3D ? ? ? ? ?");

		if (g_local_player && g_local_player->is_alive())
		{
			if (_ReturnAddress() == OnRenderStart_Return)
			{
				static auto set_abs_angles = utils::pattern_scan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8");
				//reinterpret_cast<void(__thiscall*)(void*, const QAngle&)>(set_abs_angles)(g_local_player, QAngle(0.0f, g_AnimState.m_flGoalFeetYaw, 0.0f));
			}
			else if (_ReturnAddress() == FrameNetUpdateEnd_Return)
			{
				skins::on_frame_stage_notify(true);
			}
		}

		ofunc(g_spatial_partition, a2, a3);
	}
}