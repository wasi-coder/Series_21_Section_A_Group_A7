#include "memory.h"
#include "types.h"

extern uint32_t __kernel_end; // Defined in link.ld 
static uint32_t next_free_address;

void memory_init(void) {
    next_free_address = (uint32_t)&__kernel_end;
}

void* kalloc(size_t size) {
    next_free_address = (next_free_address + 4095) & ~4095; // 4KB alignment
    void* ptr = (void*)next_free_address;
    next_free_address += size;
    return ptr;
}