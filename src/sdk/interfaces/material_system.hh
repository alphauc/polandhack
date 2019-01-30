#pragma once
#include "base_interface.hh"
#include "material.hh"

/* vfunc caller c++17 */
template<unsigned int IIdx, typename TRet, typename ... TArgs>
static auto vfunc( void* thisptr, TArgs ... argList ) -> TRet {
	using Fn = TRet( __thiscall* )( void*, decltype( argList )... );
	return ( *static_cast< Fn** >( thisptr ) )[ IIdx ]( thisptr, argList... );
}

/* vfunc caller (That's old. Some functions might not work using the c++17 version, so that's why we need this \shrug) */
template< typename Function > Function ffff( PVOID Base, DWORD Index ) {
	PDWORD* VTablePointer = ( PDWORD* )Base;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[ Index ];
	return ( Function )( dwAddress );
}

namespace sdk {

	/* <HOLY FUCKING PASTE> */
	struct texture_t {

		IDirect3DTexture9* get_texture_d3d( ) {
			return *reinterpret_cast< IDirect3DTexture9** >( uintptr_t( this ) + 0xC );
		}
	};

	class texture {
	public:

		texture_t** get_texture_d3d_handle( ) {
			return reinterpret_cast< texture_t** >( uintptr_t( this ) + 0x50 );
		}

		int get_actual_width( ) {
			return vfunc<3, int>( this );
		}

		int get_actual_height( ) {
			return vfunc<4, int>( this );
		}

	};

	struct stencil_state {

		bool enable;
		int fail;
		int zfail;
		int pass;
		int compare;
		int ref;
		int mask1;
		int mask2;
	};

	class material_render_context {
	public:

		int release( ) {
			return vfunc<1, int>( this );
		}

		void set_render_target( texture* p_texture ) {
			return vfunc<6, void>( this, p_texture );
		}

		void draw_screen_space_rectangle( material* p_material, int dest_x, int dest_y, int width, int height,
			float src_texture_x1, float src_texture_y1, float src_texture_x2, float src_texture_y2,
			int src_texture_width, int src_texture_height, void* p_client_renderable, int x_dice, int y_dice ) {

			return vfunc<113, void>( this, p_material, dest_x, dest_y, width, height, src_texture_x1, src_texture_y1,
				src_texture_x2, src_texture_y2, src_texture_width, src_texture_height, p_client_renderable, x_dice, y_dice );
		}

		void push_render_target_and_viewport( ) {
			return vfunc<118, void>( this );
		}

		void pop_render_target_and_viewport( ) {
			return vfunc<119, void>( this );
		}

		void set_stencil_state( stencil_state &state ) {
			typedef void( __thiscall* fn )( void*, stencil_state& );
			return ffff<fn>( this, 127 )( this, state );
		}
	};
	/* </HOLY FUCKING PASTE> */

	class material_system : public base_interface { // chuj ci w dupe czapek 
	public:
		material * find_material(char const* material_name, const char* group_name, bool complain = true, const char* complain_prefix = 0) {
			using original_fn = material * (__thiscall*)(material_system*, char const*, const char*, bool, const char*);
			return (*(original_fn**)this)[84](this, material_name, group_name, complain, complain_prefix);
		}
		material_handle_t first_material() {
			using original_fn = material_handle_t(__thiscall*)(material_system*);
			return (*(original_fn**)this)[86](this);
		}
		material_handle_t next_material(material_handle_t handle) {
			using original_fn = material_handle_t(__thiscall*)(material_system*, material_handle_t);
			return (*(original_fn**)this)[87](this, handle);
		}
		material_handle_t invalid_material_handle() {
			using original_fn = material_handle_t(__thiscall*)(material_system*);
			return (*(original_fn**)this)[88](this);
		}
		material* get_material(material_handle_t handle) {
			using original_fn = material * (__thiscall*)(material_system*, material_handle_t);
			return (*(original_fn**)this)[89](this, handle);
		}
		int	get_materials_count() {
			using original_fn = int(__thiscall*)(material_system*);
			return (*(original_fn**)this)[90](this);
		}
		material_render_context* get_render_context( ) {
			using original_fn = material_render_context *( __thiscall* )( material_system* );
			return ( *( original_fn** )this )[ 115 ]( this );
		}
	};
}
