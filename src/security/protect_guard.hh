#pragma once

#include <windows.h>
#include <stdexcept>
#include <cstdint>
#include <memory>

namespace security {

    class protect_guard {
    private:
        void *base_;
        size_t length_;
        uint32_t old_;

    public:
        protect_guard(void *base, size_t length, uint32_t flags);
        ~protect_guard();

    };

}