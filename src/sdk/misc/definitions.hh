#pragma once

#include <d3dx9.h>
#include <vector>
#include "vector.hh"

namespace sdk {

    using d3d_device_9 = IDirect3DDevice9;
    using d3d_font = ID3DXFont;

    using d3d_state_block = IDirect3DStateBlock9;
    using d3d_texture = IDirect3DTexture9;
	using d3d_sprite = LPD3DXSPRITE;
    using d3d_vertex_buffer = IDirect3DVertexBuffer9;

    using d3d_present_parameters = D3DPRESENT_PARAMETERS;

    using vertex = struct {
        float x, y, z, r;
        uintptr_t colour;
    };

	struct vertex_t {
		vertex_t( ) { }
		vertex_t( const vec2 &pos, const vec2 &coord = vec2( 0, 0 ) ) {
			position = pos;
			m_TexCoord = coord;
		}
		void Init( const vec2 &pos, const vec2 &coord = vec2( 0, 0 ) ) {
			position = pos;
			m_TexCoord = coord;
		}

		vec2	position;
		vec2	m_TexCoord;
	};



    using vertices = std::vector<vertex>;
}