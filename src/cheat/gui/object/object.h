#pragma once
#include <functional>

namespace object {
	class c_object;

	// Time-savers.
	using c_child = std::shared_ptr< c_object >;
	using c_cond = std::function< bool( ) >;

	using c_child_container = std::vector< c_child >;

	// Space each object 8px from each other.
	constexpr int object_spacing{ 8 };

	// Ah, yes, the ordinary boring object.
	class c_object : public std::enable_shared_from_this< c_object > {
	protected:
		// The object is set somewhere on the screen.
		sdk::vec2 m_pos;
		sdk::vec2 m_size;

		// The object may have a parent.
		c_child m_parent;

		// This can be used to look up the object in a search function.
		fnv::hash m_identifier;

		// A condition can be applied on the object to enable/disable it.
		c_cond m_condition;

		// The control might have a name.
		char m_name[ 256 ];

		// The control should be visible if we want to draw it.
		bool m_visible;

		// The control may be blocked from receiving input.
		bool m_blocked = false;
	public:
		// This can be overriden to draw the desired control.
		virtual void draw( ) {};

		// This can be overriden to update the control's characteristics.
		virtual void update( ) {};

		// These can be overriden to force controls to draw within a specific panel.
		virtual sdk::vec2 get_pos( ) { return m_pos; }
		virtual sdk::vec2 get_size( ) { return m_size; }
		virtual std::string get_name( ) { return m_name; }

		bool is_visible( ) {
			if ( m_condition )
				return m_condition( );

			return m_visible;
		}

		void set_condition( c_cond condition ) {
			m_condition = condition;
		}

		void set_identifier( fnv::hash identifier ) {
			m_identifier = identifier;
		}

		void set_name( const char *name ) {
			strcpy_s< 256 >( m_name, name );
		}

		void set_visible( bool visible ) {
			m_visible = visible;
		}

		void set_blocked( bool block ) {
			m_blocked = block;
		}

		void set_blocked_r( bool block ) {
			for ( auto &it : m_parent->container( ) ) {
				if ( it == shared_from_this( ) )
					continue;

				it->set_blocked( block );
			}
		}

		c_child top_parent( ) {
			for ( auto &it = m_parent; ; it = it->m_parent ) {
				if ( !it->m_parent || it->m_identifier == g_fnv_str( "FORM" ) )
					return it;
			}
			// return shared_from_this( );
		}

		c_child get_parent( ) {
			return m_parent;
		}

		// This is so gay, on SO MANY levels.
		virtual c_child_container &container( ) {
			static c_child_container bad{};
			return bad;
		}

		virtual size_t children( ) {
			return 0;
		}

		// We want our container/tab class to be able to access protected variables.
		friend class c_container;

		friend class c_tab;
		friend class c_tab_container;
	};

	// Let's spice the object up, turn it into a container.
	class c_container : public c_object {
	protected:
		c_child_container m_container;

	public:
		c_child find( fnv::hash id ) {
			// Easy lookups.
			for ( auto &it : m_container ) {
				if ( it->m_identifier == id )
					return it;
			}

			return shared_from_this( );
		}

		virtual c_child push( c_child object ) {
			object.get( )->m_parent = shared_from_this( );
			m_container.emplace_back( object );
			return shared_from_this( );
		}

		// Draw all members of the container, by default.
		void draw( ) {
			// Reverse iterate, avoid some issues with things like combo-boxes.
			for ( auto it = m_container.rbegin( ); it != m_container.rend( ); it++ ) {
				if ( g_config.menu.m_open )
					( *it )->update( );

				( *it )->draw( );
			}
		}

		c_child_container &container( ) {
			return m_container;
		}

		size_t children( ) {
			return m_container.size( );
		}
	};

	using c_obj_vector = std::shared_ptr< c_container >;
}