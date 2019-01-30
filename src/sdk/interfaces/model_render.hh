#pragma once

#include "base_interface.hh"
#include "material.hh"

namespace sdk {
	class model_render {
	public:
		void override_material(material* _material) {
			using original_fn = void(__thiscall*)(model_render*, material*, int, int);
			return (*(original_fn**)this)[1](this, _material, 0, 0);
		}
	};

}