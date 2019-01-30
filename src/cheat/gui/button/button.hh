#pragma once

namespace object {
	class c_button : public c_object {
	protected:
		std::function< void( ) > m_function;
		bool m_clicked = false;
	public:
		c_button( const char *name, std::function< void( ) > callback ) {
			m_function = callback;
			set_name( name );
			set_visible( true );

			m_size = { 155, 20 };

			set_identifier(g_fnv_str("BUTTON") );
		}

		void update( ) {
			if( !m_parent )
				set_visible( false );

			if( !is_visible( ) )
				return;

			// Update the position.
			m_pos.x = m_parent->get_pos( ).x + 30;
			m_pos.y = m_parent->get_pos( ).y + 10;

			for( auto &it : m_parent->container( ) ) {
				if( it == shared_from_this( ) )
					break;
				 
				if( it->is_visible( ) )
					m_pos.y += it->get_size( ).y + object_spacing;
			}

			// Don't affect the object until it's unblocked.
			if( m_blocked )
				return;

			// See if the button was clicked.
			if( g_input.key_down( VK_LBUTTON ) ) {
				if( g_input.is_in_box( m_pos, m_size ) && !m_clicked ) {
					m_clicked = true;
					m_function( );
				}
			} else {
				m_clicked = false;
			}
		}
		
		void draw( ) {
			if( !is_visible( ) )
				return;

			// Draw the button.
			render::gradient(m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR( 30, 30, 30 ), MENU_COLOR( 45, 45, 45 ), true );
			render::filled_box( m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR( 0, 0, 0 ) );

			// Draw the button name.
			render::text(render::fonts::menu, m_pos.x + 10, m_pos.y + 4, MENU_COLOR(215, 215, 215), render::alignment::left, m_name);
		}
	};
}