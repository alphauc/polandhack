#include "fnv.hh"

std::function<fnv::hash(std::string)> g_fnv_str = std::function<fnv::hash(std::string)>([&](std::string data) -> fnv::hash {
    const char *str = data.c_str();
    fnv::hash hash = fnv::hash_runtime(str);

    return hash;
});