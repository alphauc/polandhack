#include "protect_guard.hh"

std::string format(const char *fmt, ...) {
    char buf[1024];
    va_list arguments;

    va_start(arguments, fmt);
    _vsnprintf_s(buf, sizeof buf, fmt, arguments);
    va_end(arguments);
    
    return std::string(buf);
}

security::protect_guard::protect_guard(void *base, size_t length, uint32_t flags) {
    this->base_ = base;
    this->length_ = length;

    if (!VirtualProtect(this->base_, this->length_, flags, (unsigned long *)&this->old_)) {
        throw std::runtime_error(
            format(
                "failed to protect region of size %ul at 0x%8X...",
                (long) this->length_,
                (long) this->base_
            )
        );
    }
}

security::protect_guard::~protect_guard() {
    VirtualProtect(this->base_, this->length_, this->old_, (unsigned long *)&this->old_);
}