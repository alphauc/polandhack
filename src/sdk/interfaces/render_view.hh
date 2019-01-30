#pragma once
#include "base_interface.hh"
#include "../misc/colour.hh"

namespace sdk {

	class render_view : public base_interface {
	public:

		auto modulate_colour(sdk::colour colour) -> void {
			float blend[3] = { (float)(colour.r / 255.f), (float)(colour.g / 255.f), (float)(colour.b / 255.f) };

			this->get_vfunc<6, void*, float*>(this, blend);
		};

		auto blend( float alpha ) -> void {
			this->get_vfunc<4, void*, float>( this, alpha );
		};

	};

}
