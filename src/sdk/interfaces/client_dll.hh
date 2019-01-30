#pragma once

#include "../misc/recv.hh"
#include "base_interface.hh"

namespace sdk {
	enum client_framestage_t {
		FRAME_UNDEFINED = -1,
		FRAME_START,
		FRAME_NET_UPDATE_START,
		FRAME_NET_UPDATE_POSTDATAUPDATE_START,
		FRAME_NET_UPDATE_POSTDATAUPDATE_END,
		FRAME_NET_UPDATE_END,
		FRAME_RENDER_START,
		FRAME_RENDER_END
	};


    class client_dll : public base_interface {
    public:
        auto get_classes() -> client_class * { return this->get_vfunc<8, client_class *>(this); };

    };

}