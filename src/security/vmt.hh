#pragma once

#include <windows.h>
#include <cstdint>

namespace security {

    class vmt_hook {
    private:
        size_t table_length_;
        bool is_allocated_;

        void *class_base_;
        uintptr_t *new_table_;
        uintptr_t *old_table_;

        size_t estimate_table_length(uintptr_t *table_start);

    public:
        explicit vmt_hook(void *class_base);
        ~vmt_hook();

        template <size_t index, typename fn>
        void swap_fn(fn function) {
            if (index < 0 || index > this->table_length_)
                return;

            this->new_table_[index + 1] = reinterpret_cast<uintptr_t>(function);
        }

        template <typename fn>
        fn get_original(const size_t index) {
            return reinterpret_cast<fn>(this->old_table_[index]);
        }

    };

}