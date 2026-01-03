#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"

void memory_init(void);
void* kalloc(size_t size); // Simple bump allocator

#endif