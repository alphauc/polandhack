#pragma once
#include "../sdk/misc/definitions.hh"
#include "../sdk/misc/vector.hh"

namespace input {
	class c_input_helper {
	protected:
		HWND m_wnd;

		bool m_down_keys[256];
		bool m_old_keys[256];

		bool m_blocked = false;

	public:
		WNDPROC m_wndproc;
		POINT	m_cursor;
		float	m_scroll;

		// Place a hook on the window callback.
		void initialise( );

		// Process a message.
		void process( uint32_t msg, uint32_t wparam, uint32_t lparam );

		// Update old keys, etc on new frame.
		void on_frame( );

		// Finally, some non-homosexual wrappers for this stuff.
		bool key_down( uint32_t code ) {
			return m_down_keys[code];
		}
		
		bool key_click( uint32_t code ) {
			bool click = !key_down( code ) && m_old_keys[code];
			return click;
		}

		bool is_in_box(sdk::vec2 box_pos, sdk::vec2 box_size );

		void shutdown();

		void block( ) {
			m_blocked = true;
		}

		void unblock( ) {
			m_blocked = false;
		}

		bool blocked( ) {
			return m_blocked;
		}
	};
}

extern input::c_input_helper g_input;