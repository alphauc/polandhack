#pragma once
#include "../misc/vector.hh"
#include "../misc/colour.hh"

namespace sdk {
	class glow_object_manager {
	public:
		class glow_object {
		public:
			void draw(colour color) {
				colour = vec3(color.r / 255.f, color.g / 255.f, color.b / 255.f);
			}

			base_entity* get_entity() {
				return p_entity;
			}

			int set_style(int number) {
				return glow_style = number;
			}

			bool is_empty() const {
				return m_nNextFreeSlot != glow_object::ENTRY_IN_USE;
			}

		public:
			base_entity* p_entity; //0x0000 
			vec3 colour;
			float glow_alpha; //0x0010 
			int unknown01; //0x0014 
			int unknown02; //0x0018 
			float m_flBloomAmount; //0x001C 
			int unknown03; //0x0020 
			bool render_when_occluded; //0x0024
			bool render_when_unoccluded; //0x0025 
			bool full_bloom; //0x0026
			char unknown04; //0x0027  
			int m_nFullBloomStencilTestValue; // 0x0028;
			int32_t glow_style; //0x002C
			int m_nSplitScreenSlot; //0x0030 
			int m_nNextFreeSlot; //0x0034 

			static const int END_OF_FREE_LIST = -1;
			static const int ENTRY_IN_USE = -2;
		};

		glow_object* definitions;
		int	max_size;
		int	pad;
		int	size;
		int	currentObjects;
	};
}