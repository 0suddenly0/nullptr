#include "../hooks.h"

namespace hooks
{
	bool __stdcall is_playing_demo::hook()
	{
		if (settings::misc::reveal_money
			&& *static_cast<uintptr_t*>(_ReturnAddress()) == 0x0975C084  // client_panorama.dll : 84 C0 75 09 38 05
			&& **reinterpret_cast<uintptr_t**>(uintptr_t(_AddressOfReturnAddress()) + 4) == 0x0C75C084) { // client_panorama.dll : 84 C0 75 0C 5B
			return true;
		}
		auto oisPlayingDemo = engine_vhook.get_original<fn>(index::is_playing_demo);
		return false;
	}
}
