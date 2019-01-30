#pragma once

#include <cstdint>
#include "vector.hh"

namespace sdk {

    enum class prop_type {
        int32 = 0,
        flt,
        vector3,
        vector2,
        string,
        array,
        table,
        int64
    };

    class d_variant {
    public:
        union {
            float float_;
            int32_t int_;
            char *string_;
            void *data;
            sdk::vec3 vector;
            int64_t long_;

        };

        prop_type type;

    };

    class recv_table;
    class recv_prop;

    class recv_proxy_data {
    public:
        recv_prop *prop;
        d_variant value;
        int element;
        int object_id;

    };

    using recv_var_proxy_fn = void( *)(recv_proxy_data *, void *, void *);
    using array_length_proxy_fn = void( *)(void *, int, int);
    using data_table_recv_var_proxy_fn = void( *)(recv_prop *, void **, void *, int);

    class recv_prop {
    public:
        char *name;
        prop_type type;
        int flags;
        int string_size;
        int inside_array;
        const void *extra_data;
        recv_prop *array_prop;
        array_length_proxy_fn array_length_proxy;
        recv_var_proxy_fn recv_var_proxy;
        data_table_recv_var_proxy_fn data_table_recv_var_proxy;
        recv_table *data_table;
        int offset;
        int element_stride;
        int elements;
        const char *parent_prop_array_name;

    };

    class recv_table {
    public:
        recv_prop *props;
        int prop_count;
        void *decoder;
        char *name;
        bool initialized;
        bool in_main_list;

    };

    class client_class {
    public:
        void *create_fn;
        void *create_event_fn;
        char *name;
        recv_table *table;
        client_class *next;
        int class_id;

    };

}