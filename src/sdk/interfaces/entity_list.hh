#pragma once

#include "../classes/entity.hh"
#include "base_interface.hh"

namespace sdk {

    class entity_list : public base_interface {
    public:
        auto get_entity(int32_t index) -> base_entity * { return this->get_vfunc<3, base_entity *, int32_t>(this, index); };
        auto get_entity(uintptr_t handle) -> base_entity * { return this->get_vfunc<4, base_entity *, uintptr_t>(this, handle); };
        auto get_entity_count() -> int32_t { return this->get_vfunc<6, int32_t>(this); };

    };

}