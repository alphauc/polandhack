#include "netvar_manager.hh"
#include "interface_manager.hh"

std::map<fnv::hash, netvars::stored_data> netvars::props;

void netvars::initialize() {
    for (auto client_class = interfaces::client_dll->get_classes(); client_class; client_class = client_class->next) {
        if (client_class->table)
            dump_recursive(client_class->name, client_class->table, 0);
    }
}

void netvars::dump_recursive(std::string base_class, sdk::recv_table *table, uint16_t offset) {
    for (auto i = 0; i < table->prop_count; ++i) {
        const auto prop_ptr = &table->props[i];

        if (!prop_ptr || isdigit(prop_ptr->name[0]))
            continue;

        if (fnv::hash_runtime(prop_ptr->name) == g_fnv_str("baseclass"))
            continue;

        if (prop_ptr->type == sdk::prop_type::table && prop_ptr->data_table != nullptr && prop_ptr->data_table->name[0] == 'D')
            dump_recursive(base_class, prop_ptr->data_table, uint16_t(offset + prop_ptr->offset));

        char hash_name[256];

        strcpy_s(hash_name, base_class.c_str());
        strcat_s(hash_name, "->");
        strcat_s(hash_name, prop_ptr->name);

        const auto hash = fnv::hash_runtime(hash_name);
        const auto total_offset = uint16_t(offset + prop_ptr->offset);

        props[hash] = {
            prop_ptr,
            total_offset
        };
    }
}

uint16_t netvars::get_offset(const fnv::hash hash) {
    for (const auto &pair : props) {
        if (pair.first == hash)
            return pair.second.relative_offset;
    }

    return 0;
}

sdk::recv_prop *netvars::get_prop(const fnv::hash hash) {
    for (const auto &pair : props) {
        if (pair.first == hash)
            return pair.second.prop;
    }

    return nullptr;
}