#include "input.h"
#include "../menu/menu.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../valve_sdk/sdk.h"


bool is_mouse(int vk)
{
	if (vk == 1)
	{
		return true;
	}
	else if (vk == 2)
	{
		return true;
	}
	else if (vk == 4)
	{
		return true;
	}
	else if (vk == 5)
	{
		return true;
	}
	else if (vk == 6)
	{
		return true;
	}
	return false;
}

bool mouse_key(int vk, bool down)
{
	if (down)
	{
		if (vk == 1)
		{
			return ImGui::IsMouseDown(0);
		}
		else if (vk == 2)
		{
			return ImGui::IsMouseDown(1);
		}
		else if (vk == 4)
		{
			return ImGui::IsMouseDown(2);
		}
		else if (vk == 5)
		{
			return ImGui::IsMouseDown(3);
		}
		else if (vk == 6)
		{
		   return ImGui::IsMouseDown(4);
		}
	}
	else
	{
		if (vk == 1)
		{
			return ImGui::IsMouseReleased(0);
		}
		else if (vk == 2)
		{
			return ImGui::IsMouseReleased(1);
		}
		else if (vk == 4)
		{
			return ImGui::IsMouseReleased(2);
		}
		else if (vk == 5)
		{
			return ImGui::IsMouseReleased(3);
		}
		else if (vk == 6)
		{
			return ImGui::IsMouseReleased(4);
		}
	}
	return false;
}


namespace input_sys
{
	void remove()
	{
		if (m_ulOldWndProc)
			SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, m_ulOldWndProc);
		m_ulOldWndProc = 0;
	}

	void initialize()
	{
		D3DDEVICE_CREATION_PARAMETERS params;

		if (FAILED(g_d3ddevice9->GetCreationParameters(&params)))
			throw std::runtime_error("[InputSys] GetCreationParameters failed.");

		m_hTargetWindow = params.hFocusWindow;
		m_ulOldWndProc = SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, (LONG_PTR)wnd_proc);

		if (!m_ulOldWndProc)
			throw std::runtime_error("[InputSys] SetWindowLongPtr failed.");
	}

	LRESULT __stdcall wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		process_message(msg, wParam, lParam);

		if (ImGui_ImplWin32_WndProc_handler(hWnd, msg, wParam, lParam) && menu::is_visible())
			return true;

		return CallWindowProc((WNDPROC)m_ulOldWndProc, hWnd, msg, wParam, lParam);
	}

	bool process_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
		case WM_XBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
		case WM_XBUTTONUP:
			return process_mouse_message(uMsg, wParam, lParam);
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			return process_keybd_message(uMsg, wParam, lParam);
		default:
			return false;
		}
	}

	bool process_mouse_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto key = VK_LBUTTON;
		auto state = KeyState::None;
		switch (uMsg) {
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			state = uMsg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
			key = VK_MBUTTON;
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			state = uMsg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
			key = VK_RBUTTON;
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			state = uMsg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
			key = VK_LBUTTON;
			break;
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			state = uMsg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
			key = (HIWORD(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
			break;
		default:
			return false;
		}

		if (state == KeyState::Up && m_iKeyMap[key] == KeyState::Down)
			m_iKeyMap[key] = KeyState::Pressed;
		else
			m_iKeyMap[key] = state;
		return true;
	}

	bool process_keybd_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto key = wParam;
		auto state = KeyState::None;

		switch (uMsg) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			state = KeyState::Down;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			state = KeyState::Up;
			break;
		default:
			return false;
		}

		if (state == KeyState::Up && m_iKeyMap[int(key)] == KeyState::Down) {
			m_iKeyMap[int(key)] = KeyState::Pressed;

			auto& hotkey_callback = m_Hotkeys[key];

			if (hotkey_callback)
				hotkey_callback();

		}
		else {
			m_iKeyMap[int(key)] = state;
		}

		return true;
	}

	KeyState get_key_state(std::uint32_t vk)
	{
		return m_iKeyMap[vk];
	}


	bool is_key_down(std::uint32_t vk)
	{
		//return m_iKeyMap[vk] == KeyState::Down;
		if (is_mouse(vk))
		{
			return mouse_key(vk, true);
		}
		else
		{
			return ImGui::IsKeyDown(vk);
		}
	}
	bool was_key_pressed(std::uint32_t vk)
	{
		if (is_mouse(vk))
		{
			return mouse_key(vk, false);
		}
		else
		{
			return ImGui::IsKeyPressed(vk, false);
		}
	}

	void register_hotkey(std::uint32_t vk, std::function<void(void)> f)
	{
		m_Hotkeys[vk] = f;
	}
	void remove_hotkey(std::uint32_t vk)
	{
		m_Hotkeys[vk] = nullptr;
	}
}