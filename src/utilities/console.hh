#pragma once

#include <windows.h>
#include <iostream>

namespace console {

    void allocate();
    void detach();

    void title(const char *title);
    void log(const char *format, ...);

}