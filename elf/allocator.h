#ifndef __ALLOCATOR_H
#define __ALLOCATOR_H

#include "./elfreader.h"

void* alloc_memory(struct elfinfo* elf);
void load_segments(void* mem_start, struct elfinfo* elf);

#endif