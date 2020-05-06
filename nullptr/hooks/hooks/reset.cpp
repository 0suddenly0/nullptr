#include "../hooks.h"

namespace hooks
{
	long __stdcall reset::hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters) {
		auto oReset = direct3d_vhook.get_original<fn>(index::reset);

		menu::on_device_lost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
			menu::on_device_reset();

		return hr;
	}
}