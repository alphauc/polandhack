#include "input.hh"

input::c_input_helper g_input;
#include "../cheat/cfg.hh"

namespace hooked {
	long __stdcall wnd_proc(HWND wnd, uint32_t msg, uint32_t wparam, uint32_t lparam) {
		g_input.process(msg, wparam, lparam);

		if (g_config.menu.m_open)
			return true;

		return CallWindowProcA(g_input.m_wndproc, wnd, msg, wparam, lparam);
	}
}

namespace input {
	void c_input_helper::initialise() {
		// Find the window class.
		while (!m_wnd)
			m_wnd = FindWindowA("Valve001", 0);

		// We have the window, hook the window procedure.
		m_wndproc = (WNDPROC)SetWindowLongA(m_wnd, GWL_WNDPROC, (long)hooked::wnd_proc);
	}

	void c_input_helper::process(uint32_t msg, uint32_t wparam, uint32_t) {
		switch (msg) {
		case WM_LBUTTONDOWN:
			m_down_keys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			m_down_keys[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			m_down_keys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			m_down_keys[VK_RBUTTON] = false;
			break;
		case WM_MBUTTONDOWN:
			m_down_keys[VK_MBUTTON] = true;
			break;
		case WM_MBUTTONUP:
			m_down_keys[VK_MBUTTON] = false;
			break;
		case WM_MOUSEWHEEL:
			m_scroll += GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? +1 : -1;
			break;
		case WM_XBUTTONDOWN:
		{
			UINT button = GET_XBUTTON_WPARAM(wparam);
			if (button == XBUTTON1) {
				m_down_keys[VK_XBUTTON1] = true;
			}
			else if (button == XBUTTON2) {
				m_down_keys[VK_XBUTTON2] = true;
			}
			break;
		}
		case WM_XBUTTONUP:
		{
			UINT button = GET_XBUTTON_WPARAM(wparam);
			if (button == XBUTTON1) {
				m_down_keys[VK_XBUTTON1] = false;
			}
			else if (button == XBUTTON2) {
				m_down_keys[VK_XBUTTON2] = false;
			}
			break;
		}
		case WM_KEYDOWN:
			m_down_keys[wparam] = true;
			break;
		case WM_KEYUP:
			m_down_keys[wparam] = false;
			break;
		case WM_SYSKEYDOWN:
			if (wparam == VK_MENU)
				m_down_keys[VK_MENU] = true;
			break;
		case WM_SYSKEYUP:
			if (wparam == VK_MENU)
				m_down_keys[VK_MENU] = false;
			break;
		default: break;
		}
	}

	void c_input_helper::on_frame() {
		// Update pointer position.
		GetCursorPos(&m_cursor);
		ScreenToClient(m_wnd, &m_cursor);

		// Copy over old key states.
		std::memcpy(m_old_keys, m_down_keys, sizeof m_down_keys);
	}

	bool c_input_helper::is_in_box(sdk::vec2 box_pos, sdk::vec2 box_size) {
		return (
			m_cursor.x > box_pos.x &&
			m_cursor.y > box_pos.y &&
			m_cursor.x < box_pos.x + box_size.x &&
			m_cursor.y < box_pos.y + box_size.y
			);
	}

	void c_input_helper::shutdown() {
		SetWindowLongPtrA(FindWindow("Valve001", NULL), GWL_WNDPROC, (long)m_wndproc);
	}
}