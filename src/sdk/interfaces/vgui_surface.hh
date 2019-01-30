#pragma once

#include <cstdarg>
#include <codecvt>
#include <corecrt_math_defines.h>

#include "../misc/colour.hh"
#include "../misc/vector.hh"
#include "../misc/definitions.hh"
#include "../misc/vgui.hh"
#include "base_interface.hh"

namespace sdk {

	class vgui_surface : public base_interface {
	public:
		auto set_colour( int r, int g, int b, int a ) -> void { this->get_vfunc<15, void, int, int, int, int>( this, r, g, b, a ); };
		auto set_colour( colour col ) -> void { this->set_colour( col.r, col.g, col.b, col.a ); };
		auto set_texture_colour( int id, const unsigned char *rgba, int wide, int tall ) -> void {
			this->get_vfunc<37, void, const unsigned char *,
				int, int>( this, rgba, wide, tall );
		};

		auto set_text_colour( int r, int g, int b, int a ) -> void { this->get_vfunc<25, void, int, int, int, int>( this, r, g, b, a ); };
		auto set_text_colour( colour col ) -> void { this->set_text_colour( col.r, col.g, col.b, col.a ); };
		auto set_texture( int id ) -> void { this->get_vfunc<38, void, int>( this, id ); };
		auto draw_textured_box( int x, int y, int x1, int y1 ) -> void { this->get_vfunc<41, void, int, int, int, int>( this, x, y, x1, y1 ); };

		auto set_text_position( int x, int y ) -> void { this->get_vfunc<26, void, int, int>( this, x, y ); };
		auto set_text_position( vec2 pos ) -> void { this->set_text_position( static_cast< int >( pos.x ), static_cast< int >( pos.y ) ); };

		auto set_text_font( vgui::font font ) -> void { this->get_vfunc<23, void, vgui::font>( this, font ); };

		auto draw_text( std::wstring text ) -> void { this->get_vfunc<28, void, const wchar_t *, size_t, int>( this, text.c_str( ), text.size( ), 0 ); };

		auto create_new_texture_id( bool procedural = false ) -> int { this->get_vfunc<42, void, bool>( this, procedural ); };

		auto draw_text( const char *format, vec2 pos, colour color, vgui::font font, ... ) -> void {
			char buf[ 1024 ];
			va_list args;

			va_start( args, format );
			_vsnprintf_s( buf, sizeof buf, format, args );
			va_end( args );

			static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			this->set_text_colour( color );
			this->set_text_position( pos );
			this->set_text_font( font );
			this->draw_text( converter.from_bytes( buf ) );
		};

		auto get_text_size( vgui::font font, const wchar_t* text, int& wide, int& tall ) -> void {
			this->get_vfunc<79, void*, vgui::font, const wchar_t*, int&, int&>(
				this, font, text, wide, tall
				);
		};

		auto fill_rect( int x, int y, int w, int h ) -> void { this->get_vfunc<16, void, int, int, int, int>( this, x, y, x + w, y + h ); };
		auto fill_rect( vec2 pos, vec2 size ) -> void { this->fill_rect( static_cast< int >( pos.x ), static_cast< int >( pos.y ), static_cast< int >( size.x ), static_cast< int >( size.y ) ); };

		auto outline_rect( int x, int y, int w, int h ) -> void { this->get_vfunc<18, void, int, int, int, int>( this, x, y, x + w, y + h ); };
		auto outline_rect( vec2 pos, vec2 size ) -> void { this->outline_rect( static_cast< int >( pos.x ), static_cast< int >( pos.y ), static_cast< int >( size.x ), static_cast< int >( size.y ) ); };

		auto line( int x1, int y1, int x2, int y2 ) -> void { this->get_vfunc<19, void, int, int, int, int>( this, x1, y1, x2, y2 ); };
		auto line( vec2 start, vec2 end ) -> void { this->line( static_cast< int >( start.x ), static_cast< int >( start.y ), static_cast< int >( end.x ), static_cast< int >( end.y ) ); };

		void polygon( int n, vertex *vertice, bool clip_vertices = true ) {
			using original_fn = void( __thiscall* )( void*, int, vertex*, bool );
			return ( *( original_fn** )this )[ 106 ]( this, n, vertice, clip_vertices );
		}
		void polygon( int n, vertex_t *vertice, bool clip_vertices = true ) {
			using original_fn = void( __thiscall* )( void*, int, vertex_t*, bool );
			return ( *( original_fn** )this )[ 106 ]( this, n, vertice, clip_vertices );
		}

		auto allocate_font( ) -> vgui::font { return this->get_vfunc<71, vgui::font>( this ); };
		auto set_font( vgui::font font, std::string font_name, int size, int weight, int flags ) -> void {
			this->get_vfunc<72, void, vgui::font, const char *, int, int, int, int, int, int, int>(
				this, font, font_name.c_str( ), size, weight, 0, 0, flags, 0, 0
				);
		};

		auto get_cursor_pos( int &x, int &y ) -> void { this->get_vfunc<100, void, int &, int &>( this, x, y ); };
		auto get_cursor_pos( ) -> vec2 {
			int x, y;
			this->get_cursor_pos( x, y );

			return vec2(
				static_cast< float >( x ),
				static_cast< float >( y )
			);
		};

		auto lock_cursor( ) -> void { this->get_vfunc<67, void>( this ); };
		auto unlock_cursor( ) -> void { this->get_vfunc<66, void>( this ); };

		auto draw_rect( vec2 pos, vec2 size, colour color, colour outline_color = colour::black ) -> void {
			this->set_colour( outline_color );
			this->outline_rect( pos - vec2( 1, 1 ), size + vec2( 2, 2 ) );
			this->outline_rect( pos + vec2( 1, 1 ), size - vec2( 2, 2 ) );
			this->set_colour( color );
			this->outline_rect( pos, size );
		};

		auto draw_filled_rect( vec2 pos, vec2 size, colour color, colour outline_color = colour::black, bool outline = false ) -> void {
			if ( outline ) {
				this->set_colour( outline_color );
				this->outline_rect( pos - vec2( 1, 1 ), size + vec2( 2, 2 ) );
				this->outline_rect( pos + vec2( 1, 1 ), size - vec2( 2, 2 ) );
			}
			this->set_colour( color );
			this->fill_rect( pos, size );
		};

		void circle( vec2 center, colour color, float radius, float points )
		{
			static bool once = true;

			static std::vector<float> temppointsx;
			static std::vector<float> temppointsy;

			if ( once )
			{
				float step = ( float )M_PI * 2.0f / points;
				for ( float a = 0; a < ( M_PI * 2.0f ); a += step )
				{
					temppointsx.push_back( cosf( a ) );
					temppointsy.push_back( sinf( a ) );
				}
				once = false;
			}

			std::vector<int> pointsx;
			std::vector<int> pointsy;
			std::vector<vertex_t> vertices;

			for ( int i = 0; i < temppointsx.size( ); i++ )
			{
				float x = radius * temppointsx[ i ] + center.x;
				float y = radius * temppointsy[ i ] + center.y;
				pointsx.push_back( x );
				pointsy.push_back( y );

				vertices.push_back( vertex_t( vec2( x, y ) ) );
			}

			set_colour( color );
			polygon( points, vertices.data( ) );
		}
	};

}