#include "render.h"
#include <Windows.h>
#include "../hooks/hooks.h"
#include <mutex>

#include "../features/visuals/visuals.h"
#include "../valve_sdk/csgostructs.h"
#include "../helpers/input.h"
#include "../menu/menu.h"
#include "../options/options.h"
#include "../helpers/fonts/fonts.h"
#include "../valve_sdk/math/Vector2D.h"
#include "../helpers/math.h"
#include "backdrop.h"
#include "../lua sdk/lua_sdk.h"

#include <experimental/filesystem>



ImFont* small_font_2;
ImFont* default_font;
ImFont* small_font;
ImFont* esp_font;
ImFont* second_font;

ImDrawListSharedData _data;

std::mutex render_mutex;

const std::string current_date_time()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "date: %d-%m-%Y | time: %X", &tstruct);

	return buf;
}

auto framerate = 0.0f;

namespace render
{
	void initialize()
	{
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(input_sys::get_main_window());
		ImGui_ImplDX9_Init(g_d3ddevice9);

		draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
		draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
		draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());

		get_fonts();
	}

	void get_fonts()
	{
		small_font_2 = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
			fonts::droid_compressed_data,
			fonts::droid_compressed_size,
			12.f,
			nullptr,
			ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

		ImFontConfig config;
		config.SizePixels = 1.f;

		esp_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
			fonts::droid_compressed_data,
			fonts::droid_compressed_size,
			12,
			&config,
			ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());


		//watermark font
		small_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
			fonts::droid_compressed_data,
			fonts::droid_compressed_size,
			14.f,
			nullptr,
			ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

		// menu font
		ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
			fonts::droid_compressed_data,
			fonts::droid_compressed_size,
			14.f,
			nullptr,
			ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

		// esp font
		default_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
			fonts::droid_compressed_data,
			fonts::droid_compressed_size,
			18.f,
			nullptr,
			ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());


		// font for watermark; just example
		second_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
			fonts::cousine_compressed_data,
			fonts::cousine_compressed_size,
			18.f,
			nullptr,
			ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	}

	void clear_drawlist()
	{
		render_mutex.lock();
		draw_list_act->Clear();
		render_mutex.unlock();
	}

	void draw_watermark()
	{
		int screen_w;
		int screen_h;

		g_engine_client->get_screen_size(screen_w, screen_h);

		framerate = 0.9 * framerate + (1.0 - 0.9) * g_global_vars->absoluteframetime;

		std::string main_text = utils::snprintf("nullptr | v %.2f | %s | fps: %3i", NULLPTR_VERSION, current_date_time().c_str(), (int)(1.0f / framerate));
		std::string main_r_text = utils::snprintf(" | v %.2f | %s | fps: %3i", NULLPTR_VERSION, current_date_time().c_str(), (int)(1.0f / framerate));

		ImVec2 main_size = esp_font->CalcTextSizeA(12, FLT_MAX, 0.0f, main_text.c_str());
		ImVec2 textSizeDemon = esp_font->CalcTextSizeA(12, FLT_MAX, 0.0f, "null");
		ImVec2 textSizesens = esp_font->CalcTextSizeA(12, FLT_MAX, 0.0f, "ptr");

		draw_box_filled((screen_w - 20) - 280 - 5, 7, ((screen_w - 20) - 280) + (int)main_size.x + 5, (int)main_size.y + 17, Color(0, 0, 0, 150), 1.f, ImGui::GetStyle().FrameRounding);
		draw_list->AddRectFilled(ImVec2((screen_w - 20) - 280 - 5, 7), ImVec2(((screen_w - 20) - 280) + main_size.x + 5, 11), ImGui::GetColorU32(ImGuiCol_SliderGrab), ImGui::GetStyle().FrameRounding, ImDrawCornerFlags_Top);

		draw_text("null", (screen_w - 20) - 280, 13, 12.f, Color(255, 255, 255), false, false, esp_font);
		draw_text("ptr", (screen_w - 20) - 280 + (int)textSizeDemon.x + 1, 13, 12.f, settings::misc::menu_color, false, false, esp_font);

		draw_text(main_r_text.c_str(), (screen_w - 20) - 280 + (int)textSizeDemon.x + (int)textSizesens.x + 1, 13, 12.f, Color(255, 255, 255), false, false, esp_font);
	}

	void begin_scene()
	{
		draw_list->Clear();
		draw_list->PushClipRectFullScreen();

		for (auto hk : lua::hooks->get_hooks("on_paint"))
		{
			try
			{
				auto result = hk.func();
				if (!result.valid()) {
					sol::error err = result;

					notify::add("lua", err.what(), log_type::console);
					utils::console_print(err.what());
					//g_console.log(err.what());
					//g_interfaces.cvar->consoleColorPrintf(CColor(255, 0, 0), err.what());
				}
			}
			catch (const std::exception&)
			{

			}
		}

		notify::render();

		int screenWidth, screenHeight;
		g_engine_client->get_screen_size(screenWidth, screenHeight);
		if (menu::is_visible())
			draw_box_filled(0, 0, screenWidth, screenHeight, Color(0.f, 0.f, 0.f, 0.5f));

		backdrop::render();

		if (settings::misc::watermark)
		{
			draw_watermark();
		}

		if (settings::misc::disable_zoom_border)
		{
			if (g_engine_client->is_ingame() && g_local_player && g_local_player->is_alive())
			{
				auto weapon = g_local_player->active_weapon();
				if (weapon)
				{
					int wpn_idx = weapon->item().item_definition_index();
					if (wpn_idx == WEAPON_AWP || wpn_idx == WEAPON_SSG08 || wpn_idx == WEAPON_G3SG1 || wpn_idx == WEAPON_SCAR20)
					{
						if (g_local_player->is_scoped() && settings::misc::disable_zoom_border)
						{
							int x;
							int y;

							g_engine_client->get_screen_size(x, y);

							draw_box_filled((x / 2) - 1, 0, (x / 2) + 1, y, Color(0, 0, 0, 150));

							draw_box_filled(0, (y / 2) - 1, x, (y / 2) + 1, Color(0, 0, 0, 150));
						}
					}
				}
			}
		}

		if (g_engine_client->is_ingame() && g_local_player)
	    	visuals::render();

		//if (g_engine_client->is_ingame() && g_local_player)
		//	visuals::draw_fov();

		/*float val = std::clamp(Misc::desync::next_lby_update_time - g_global_vars->curtime, 0.f, 999.f);
		if (g_engine_client->IsInGame() && g_local_player && settings::misc::desync::type)
		{
			int PosDesyncY;

			PosDesyncY = 15;
			char buff[228];

			snprintf(buff, sizeof(buff), "lby = %.2f ; desy delt = %.2f", val, g_local_player->get_max_desync_delta());

			std::string text = buff;

			Render::Get().RenderText(text, ImVec2(8, 5), 12, Color(255, 255, 255, 255), false, true, g_pEspFont);
			//Render::Get().RenderText("broke lby", 8, PosDesyncY, 12.0f, Misc::desync::next_lby_update() ? Color(255,255,255,255) : Color(255,100,100, 255));
			//Render::Get().RenderText("lby null", 8, PosDesyncY + 15, 18.0f, (val >= 0.0f) ? Color(255, 255, 255, 255) : Color(255, 100, 100, 255));
		}*/

		render_mutex.lock();
		*draw_list_act = *draw_list;
		render_mutex.unlock();
	}


	ImDrawList* render_scene() {

		if (render_mutex.try_lock()) {
			*draw_list_rendering = *draw_list_act;
			render_mutex.unlock();
		}

		return draw_list_rendering;
	}
	void draw_textured_polygon(int n, std::vector<ImVec2> vertice, Color color)
	{
		draw_list->AddTriangleFilled(vertice[0], vertice[1], vertice[2], get_u32(color));
	}

	float draw_text(const std::string& text, ImVec2 pos, float size, ImVec4 color, bool center, bool outline, ImFont* pFont)
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
		if (!pFont->ContainerAtlas) return 0.f;
		//ImGui::PushFont(pFont);
		draw_list->PushTextureID(pFont->ContainerAtlas->TexID);

		/*if (pFont->ContainerAtlas->TexID == draw_list->_TextureIdStack.back())
			return 0.f;*/

		if (center)
			pos.x -= textSize.x / 2.0f;
		if (outline)
		{
			if (settings::visuals::outline_type == 0)
			{
				draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y), ImGui::GetColorU32(ImVec4(0, 0, 0, color.w)), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.w)), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.w)), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x, pos.y + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.w)), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x, pos.y - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.w)), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y), ImGui::GetColorU32(ImVec4(0, 0, 0, color.w)), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.w)), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.w)), text.c_str());
			}
			else if (settings::visuals::outline_type == 1)
			{
				draw_list->AddRectFilled(ImVec2(pos.x - 2, pos.y - 1), ImVec2(pos.x + textSize.x + 2, pos.y + textSize.y + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.4f)));
			}
		}
		draw_list->AddText(pFont, size, pos, ImGui::GetColorU32(color), text.c_str());

		//ImGui::PopFont();
		draw_list->PopTextureID();

		return pos.y + textSize.y;
	}

	float draw_text(const std::string& text, ImVec2 pos, float size, Color color, bool center, bool outline, ImFont* pFont)
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
		if (!pFont->ContainerAtlas || !pFont->ContainerAtlas->TexID)
			return 0.f;

		draw_list->PushTextureID(pFont->ContainerAtlas->TexID);
		//ImGui::PushFont(pFont);

		/*if (pFont->ContainerAtlas->TexID == draw_list->_TextureIdStack.back())
			return 0.f;*/

		if (center)
			pos.x -= textSize.x / 2.0f;
		if (outline)
		{
			if (settings::visuals::outline_type == 0)
			{
				draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + 1), get_u32(Color(0, 0, 0, color.a())), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y - 1), get_u32(Color(0, 0, 0, color.a())), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y - 1), get_u32(Color(0, 0, 0, color.a())), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + 1), get_u32(Color(0, 0, 0, color.a())), text.c_str());

				draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + 1), get_u32(Color(0, 0, 0, color.a())), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y - 1), get_u32(Color(0, 0, 0, color.a())), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y - 1), get_u32(Color(0, 0, 0, color.a())), text.c_str());
				draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + 1), get_u32(Color(0, 0, 0, color.a())), text.c_str());
			}
			else if (settings::visuals::outline_type == 1)
			{
				draw_list->AddRectFilled(ImVec2(pos.x - 2, pos.y - 1), ImVec2(pos.x + textSize.x + 2, pos.y + textSize.y + 1), get_u32(Color(0, 0, 0, 102)));
			}
		}

		draw_list->AddText(pFont, size, pos, get_u32(color), text.c_str());

		//ImGui::PopFont();
		draw_list->PopTextureID();

		return pos.y + textSize.y;
		return 0.f;
	}

	void draw_circle_3d(Vector position, float points, float radius, Color color)
	{
		float step = (float)M_PI * 2.0f / points;

		for (float a = 0; a < (M_PI * 2.0f); a += step)
		{
			Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
			Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

			Vector start2d, end2d;
			if (g_debug_overlay->screen_position(start, start2d) || g_debug_overlay->screen_position(end, end2d))
				return;

			draw_line(start2d.x, start2d.y, end2d.x, end2d.y, color);
		}
	}

}
