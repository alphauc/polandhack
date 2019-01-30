#include "vmt.hh"
#include "protect_guard.hh"

size_t security::vmt_hook::estimate_table_length(uintptr_t *table_start) {
    _MEMORY_BASIC_INFORMATION memory_info = { 0 };

    size_t size = -1;
    do {
        size++;
        VirtualQuery(reinterpret_cast<void *>(table_start[size]), &memory_info, sizeof memory_info);

    } while (memory_info.Protect == PAGE_EXECUTE_READ || memory_info.Protect == PAGE_EXECUTE_READWRITE);

    return size;
}

security::vmt_hook::vmt_hook(void *class_base) {
    this->class_base_ = class_base;
    this->old_table_ = *(uintptr_t **) class_base;

    this->table_length_ = this->estimate_table_length(this->old_table_) * sizeof uintptr_t;

    if (this->table_length_ == 0)
        return;

    this->new_table_ = new uintptr_t[this->table_length_ + 1]();
    std::memcpy(&this->new_table_[1], this->old_table_, this->table_length_);

    this->is_allocated_ = true;

    try {
        auto guard = protect_guard(this->class_base_, sizeof uintptr_t, 4);

        this->new_table_[0] = this->old_table_[-1];
        *(uintptr_t **) this->class_base_ = &this->new_table_[1];
    }
    catch (...) {
        this->is_allocated_ = false;
        delete[] this->new_table_;
    }
}

security::vmt_hook::~vmt_hook() {
    if (this->is_allocated_) {
        try {
            if (this->old_table_) {
                auto guard = protect_guard{ this->class_base_, sizeof uintptr_t, 4 };

                *(uintptr_t **) this->class_base_ = this->old_table_;
                this->old_table_ = nullptr;
            }
        }
        catch (...) {}

        delete[] this->new_table_;
        this->is_allocated_ = false;
    }
}