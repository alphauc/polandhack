#pragma once
#include "base_interface.hh"
namespace sdk {
	enum MaterialVarFlags_t {
		MATERIAL_VAR_DEBUG = (1 << 0),
		MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
		MATERIAL_VAR_NO_DRAW = (1 << 2),
		MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
		MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
		MATERIAL_VAR_VERTEXALPHA = (1 << 5),
		MATERIAL_VAR_SELFILLUM = (1 << 6),
		MATERIAL_VAR_ADDITIVE = (1 << 7),
		MATERIAL_VAR_ALPHATEST = (1 << 8),
		//MATERIAL_VAR_UNUSED = (1 << 9),
		MATERIAL_VAR_ZNEARER = (1 << 10),
		MATERIAL_VAR_MODEL = (1 << 11),
		MATERIAL_VAR_FLAT = (1 << 12),
		MATERIAL_VAR_NOCULL = (1 << 13),
		MATERIAL_VAR_NOFOG = (1 << 14),
		MATERIAL_VAR_IGNOREZ = (1 << 15),
		MATERIAL_VAR_DECAL = (1 << 16),
		MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
		MATERIAL_VAR_UNUSED = (1 << 18), // UNUSED
		MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
		MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
		MATERIAL_VAR_TRANSLUCENT = (1 << 21),
		MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
		MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
		MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
		MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
		MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
		MATERIAL_VAR_HALFLAMBERT = (1 << 27),
		MATERIAL_VAR_WIREFRAME = (1 << 28),
		MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
		MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
		MATERIAL_VAR_VERTEXFOG = (1 << 31),
	};

	// These are given to FindMaterial to reference the texture groups that Show up on the 
#define TEXTURE_GROUP_LIGHTMAP						        "Lightmaps"
#define TEXTURE_GROUP_WORLD							          "World textures"
#define TEXTURE_GROUP_MODEL							          "Model textures"
#define TEXTURE_GROUP_VGUI							          "VGUI textures"
#define TEXTURE_GROUP_PARTICLE						        "Particle textures"
#define TEXTURE_GROUP_DECAL							          "Decal textures"
#define TEXTURE_GROUP_SKYBOX						          "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				      "ClientEffect textures"
#define TEXTURE_GROUP_OTHER							          "Other textures"
#define TEXTURE_GROUP_PRECACHED						        "Precached"
#define TEXTURE_GROUP_CUBE_MAP						        "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					      "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					        "Unaccounted textures"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		  "Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			    "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			  "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			  "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					      "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					        "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					      "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				      "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			  "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					      "Morph Targets"

	using material_handle_t = unsigned short;
	class material : public base_interface {
	public:
		const char* get_name() {
			using original_fn = const char*(__thiscall*)(material*);
			return (*(original_fn**)this)[0](this);
		}
		const char* get_group_name() {
			using original_fn = const char*(__thiscall*)(material*);
			return (*(original_fn**)this)[1](this);
		}
		void set_alpha(float alpha) {
			using original_fn = void(__thiscall*)(material*, float);
			return (*(original_fn**)this)[27](this, alpha);
		}
		void set_alpha(int alpha) {
			using original_fn = void(__thiscall*)(material*, float);
			return (*(original_fn**)this)[27](this, static_cast<float>(alpha) / 255.f);
		}
		void set_alpha(int alpha, float divide) {
			using original_fn = void(__thiscall*)(material*, float);
			return (*(original_fn**)this)[27](this, static_cast<float>(alpha) / divide);
		}
		void set_color(float r, float g, float b) {
			using original_fn = void(__thiscall*)(material*, float, float, float);
			return (*(original_fn**)this)[28](this, r, g, b);
		}

		void set_color(int r, int g, int b) {
			using original_fn = void(__thiscall*)(material*, float, float, float);
			return (*(original_fn**)this)[28](this, r / 255.f, g / 255.f, b / 255.f);
		}
		void set_color(int color32) {
			int r1 = (color32 >> 16) & 0xff;
			int g1 = (color32 >> 8) & 0xff;
			int b1 = color32 & 0xff;
			this->set_color(r1, g1, b1);
		}
		void set_flag(int flag, bool on) {
			using original_fn = void(__thiscall*)(material*, int, bool);
			return (*(original_fn**)this)[29](this, flag, on);
		}
	};

}