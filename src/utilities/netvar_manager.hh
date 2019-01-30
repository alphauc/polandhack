#pragma once

#include <map>

#include "../sdk/misc/recv.hh"
#include "../security/fnv.hh"

namespace netvars {

    struct stored_data {
        sdk::recv_prop *prop;
        uint16_t relative_offset;
    };

    extern std::map<fnv::hash, stored_data> props;

    void initialize();
    void dump_recursive(std::string base_class, sdk::recv_table *table, uint16_t offset);

    uint16_t get_offset(const fnv::hash hash);
    sdk::recv_prop *get_prop(const fnv::hash hash);

    template <fnv::hash hash>
    uint16_t get_offset_cached() {
        static auto offset = uint16_t(0);

        if (!offset)
            offset = get_offset(hash);

        return offset;
    }

}