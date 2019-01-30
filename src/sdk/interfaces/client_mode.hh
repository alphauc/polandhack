#pragma once

#include "base_interface.hh"

namespace sdk {

	class client_mode : public base_interface {
		// we could just use void* instead of making 
		// a whole new class, but this looks better
		// and more consistent. ~ alpha.
	};

}