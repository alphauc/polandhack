#include "console.hh"

void console::allocate() {
    AllocConsole();

    freopen_s((_iobuf**) __acrt_iob_func(0), "conin$",  "r", (_iobuf*) __acrt_iob_func(0));
    freopen_s((_iobuf**) __acrt_iob_func(1), "conout$", "w", (_iobuf*) __acrt_iob_func(1));
    freopen_s((_iobuf**) __acrt_iob_func(2), "conout$", "w", (_iobuf*) __acrt_iob_func(2));
}

void console::detach() {
    fclose((_iobuf*) __acrt_iob_func(0));
    fclose((_iobuf*) __acrt_iob_func(1));
    fclose((_iobuf*) __acrt_iob_func(2));

    FreeConsole();
}

void console::title(const char *title) {
    SetConsoleTitleA(title);
}

void console::log(const char *format, ...) {
    char buf[1024];
    va_list arguments;

    va_start(arguments, format);
    _vsnprintf_s(buf, sizeof buf, format, arguments);
    va_end(arguments);

    std::cout << buf << std::endl;
}