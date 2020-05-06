#include "../hooks.h"

std::string bec_sky_name = "";

namespace hooks
{
	long __stdcall end_scene::hook(IDirect3DDevice9* pDevice)
	{
		auto oEndScene = direct3d_vhook.get_original< fn >(index::end_scene);

		static uintptr_t gameoverlay_return_address = 0;
		if (!gameoverlay_return_address) {
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery(_ReturnAddress(), &info, sizeof(MEMORY_BASIC_INFORMATION));

			char mod[MAX_PATH];
			GetModuleFileNameA((HMODULE)info.AllocationBase, mod, MAX_PATH);

			if (strstr(mod, "gameoverlay"))
				gameoverlay_return_address = (uintptr_t)(_ReturnAddress());
		}

		if (gameoverlay_return_address != (uintptr_t)(_ReturnAddress()))
			return oEndScene(pDevice);

		IDirect3DVertexDeclaration9* vertDec;
		IDirect3DVertexShader9* vertShader;
		pDevice->GetVertexDeclaration(&vertDec);
		pDevice->GetVertexShader(&vertShader);

		/*pDevice->SetVertexDeclaration(nullptr);
		pDevice->SetVertexShader(nullptr);*/

		static auto r_modelAmbientMin = g_cvar->find_var("r_modelAmbientMin");
		static auto mat_force_tonemap_scale = g_cvar->find_var("mat_force_tonemap_scale");
		static auto mat_postprocess_enable = g_cvar->find_var("mat_postprocess_enable");

		if (!g_unload)
		{
			if (settings::misc::nightmode)
			{
				static auto sv_skyname = g_cvar->find_var("sv_skyname");
				static convar* r_3dsky = g_cvar->find_var("r_3dsky");
				sv_skyname->m_nFlags &= ~FCVAR_CHEAT;
				if (bec_sky_name == "")
				{
					bec_sky_name = sv_skyname->get_string();
				}
				sv_skyname->set_value("sky_csgo_night02");

				if (r_3dsky != nullptr)
				{
					if (r_3dsky->m_nFlags & FCVAR_HIDDEN)
						r_3dsky->m_nFlags &= ~FCVAR_HIDDEN;

					r_3dsky->set_value(0);
				}
			}

			//r_modelAmbientMin->set_value(settings::misc::nightmode ? 1.f : 0.0f);
			mat_force_tonemap_scale->set_value(settings::misc::nightmode ? 0.2f : 1.0f);
			//mat_postprocess_enable->set_value(settings::misc::nightmode ? 1 : 0);
		}
		else
		{
			static convar* sv_skyname = g_cvar->find_var("sv_skyname");
			static convar* r_3dsky = g_cvar->find_var("r_3dsky");
			sv_skyname->m_nFlags &= ~FCVAR_CHEAT; // something something dont force convars
			sv_skyname->set_value(bec_sky_name.c_str());
			bec_sky_name = "";
			if (r_3dsky != nullptr)
			{
				r_3dsky->set_value(0);
			}

			//if (r_modelAmbientMin->get_float() != 0.0f)
			//	r_modelAmbientMin->set_value(0.0f);

			if (mat_force_tonemap_scale->get_float() != 1.0f)
				mat_force_tonemap_scale->set_value(1.0f);

			if (mat_postprocess_enable->get_float() != 0)
				mat_postprocess_enable->set_value(0);
		}

		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		static auto misc_viewmodel_fov = g_cvar->find_var("viewmodel_fov");
		static auto misc_viewmodel_fov_x = g_cvar->find_var("viewmodel_offset_x");
		static auto misc_viewmodel_fov_y = g_cvar->find_var("viewmodel_offset_y");
		static auto misc_viewmodel_fov_z = g_cvar->find_var("viewmodel_offset_z");

		misc::inverse_gravity();
		misc::fog();

		misc_viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		misc_viewmodel_fov->set_value(settings::misc::viewmodel::viewmodel);

		misc_viewmodel_fov_x->m_fnChangeCallbacks.m_Size = 0;
		misc_viewmodel_fov_x->set_value(settings::misc::viewmodel::fov_x);

		misc_viewmodel_fov_y->m_fnChangeCallbacks.m_Size = 0;
		misc_viewmodel_fov_y->set_value(settings::misc::viewmodel::fov_y);

		misc_viewmodel_fov_z->m_fnChangeCallbacks.m_Size = 0;
		misc_viewmodel_fov_z->set_value(settings::misc::viewmodel::fov_z);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto esp_drawlist = render::render_scene();
		menu::render();

		ImGui::Render(esp_drawlist);
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true/*srgbwrite*/);
		pDevice->SetVertexDeclaration(vertDec);
		pDevice->SetVertexShader(vertShader);

		return oEndScene(pDevice);
	}
}