#pragma once

#include "../misc/vgui.hh"
#include "base_interface.hh"

namespace sdk {

    class vgui_panel : public base_interface {
    public:
        auto get_name(vgui::panel panel) -> const char * { return this->get_vfunc<36, const char *, vgui::panel>(this, panel); };
    };

}