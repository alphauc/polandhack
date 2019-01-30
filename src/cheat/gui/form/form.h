#pragma once

namespace object {
	class c_form : public c_container {
	protected:
		bool	m_dragging;
		sdk::vec2	m_drag;

	public:
		c_form( int x, int y, int w, int h ) {
			m_pos = { x, y };
			m_size = { w, h };

			set_visible( true );
			set_identifier( g_fnv_str( "FORM" ) );
		}

		void update( ) {
			if ( !m_parent )
				set_visible( false );

			if ( !is_visible( ) )
				return;

			sdk::vec2 titlebar{ m_size.x, 20 };

			// Form has been released.
			if ( m_dragging && !g_input.key_down( VK_LBUTTON ) )
				m_dragging = false;

			// Update form position.
			if ( m_dragging ) {
				m_pos.x = g_input.m_cursor.x - m_drag.x;
				m_pos.y = g_input.m_cursor.y - m_drag.y;
			}

			// Check if we are dragging the form.
			if ( g_input.key_down( VK_LBUTTON ) && g_input.is_in_box( m_pos, titlebar ) ) {
				m_dragging = true;
				m_drag.x = g_input.m_cursor.x - m_pos.x;
				m_drag.y = g_input.m_cursor.y - m_pos.y;
			}
		}

		void draw( ) {
			if ( !is_visible( ) )
				return;

			// lambda to handle outlines
			auto outline = [ & ]( int offset, sdk::colour color ) {
				auto box = [ ]( int x, int y, int w, int h, sdk::colour c ) {
					render::line( x, y, x, y + h, c );
					render::line( x, y + h, x + w + 1, y + h, c );
					render::line( x + w, y, x + w, y + h, c );
					render::line( x + 1, y, x + w, y, c );
				};

				box( m_pos.x - offset, m_pos.y - offset, m_size.x + offset * 2, m_size.y + offset * 2, color );
			};


			// Draw the form background.
			render::filled_box( m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR( 4, 4, 4 ) );

			// Draw the form borders.
			outline( 0, MENU_COLOR( 57, 57, 57 ) );
			outline( 1, MENU_COLOR( 35, 35, 35 ) );
			outline( 2, MENU_COLOR( 35, 35, 35 ) );
			outline( 3, MENU_COLOR( 35, 35, 35 ) );
			outline( 4, MENU_COLOR( 57, 57, 57 ) );
			outline( 5, MENU_COLOR( 1, 1, 1 ) );

			render::filled_box( m_pos.x + 2, m_pos.y + 2, m_size.x - 3, 1, MENU_COLOUR( g_config.menu.m_accent ) );
			render::filled_box( m_pos.x + 2, m_pos.y + 3, m_size.x - 3, 1,
				MENU_COLOR(
					g_config.menu.m_accent.r * 0.40f,
					g_config.menu.m_accent.g * 0.40f,
					g_config.menu.m_accent.b * 0.40f ) );


			// Draw container children.
			c_container::draw( );
		}
	};
}