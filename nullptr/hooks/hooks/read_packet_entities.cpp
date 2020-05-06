#include "../hooks.h"

namespace hooks
{
	void __fastcall read_packet_entities::hook(void* pClientState, void* pEDX, void* pEntInfo)
	{
		static auto original = clientstate_vhook.get_original<fn>(index::read_packet_entities);

		original(pClientState, pEntInfo);

		skins::on_frame_stage_notify(false);
	}
}