#pragma once

namespace object {
	class c_tab_control : public c_container {
	public:
		c_tab_control( ) {
			// The tab control is always 100px wide.
			// Change this if you think it's too large for your menu.
			m_size.x = 99;

			set_visible( true );
			set_identifier(g_fnv_str( "TAB_CONTROLLER" ) );
		}

		void update( ) {
			// A tab control must have a parent.
			if( !m_parent )
				set_visible( false );

			// The tab control is NOT valid, or disabled.
			if( !is_visible( ) )
				return;

			// 20px from the top, 20px from the bottom.
			m_size.y = m_parent->get_size( ).y - 30;

			m_pos.x = m_parent->get_pos( ).x + 15;
			m_pos.y = m_parent->get_pos( ).y + 15;
		}

		void draw( ) {
			// Don't draw the control if it's invisible.
			if( !is_visible( ) )
				return;

			// Draw background.
			render::filled_box( m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR( 9, 9, 9 ) );

			render::bordered_box( m_pos.x, m_pos.y, m_size.x, m_size.y, MENU_COLOR( 0, 0, 0 ) );
			render::bordered_box( m_pos.x + 1, m_pos.y + 1, m_size.x - 2, m_size.y - 2, MENU_COLOR( 45, 45, 45 ) );

			// Draw any children that the tab control has.
			c_container::draw( );
		}
	};
}