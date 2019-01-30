#pragma once

#include <cstdint>

namespace sdk {

    class base_interface {
    public:
        template <size_t index, typename ret, class ... args_>
        __forceinline ret get_vfunc(void *thisptr, args_... args) {
            using fn = ret(__thiscall *)(void *, args_...);

            auto fn_ptr = (fn)(*(uintptr_t **)thisptr)[index];
            return fn_ptr(thisptr, args...);
        }

    };

}