#pragma once

#include "../misc/vector.hh"
#include "base_interface.hh"

namespace sdk {

    class debug_overlay : public base_interface {
    public:
        auto to_screen(vec3 &in, vec3 &out) -> bool { return this->get_vfunc<13, int, vec3 &, vec3 &>(this, in, out) != 1; };

    };

}