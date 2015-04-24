// Copyright 1 2

#include <string.h>

#include "./elfreader.h"

#define BUCKET_OFFSET 2

unsigned long elf_hash(const unsigned char *name) {
    unsigned long h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        if ((g = h & 0xf0000000)) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h;
}

void* resolve_symbol(void* mem, struct dyninfo* dyn, const char *name) {
    unsigned long hash = elf_hash((const unsigned char*)name);
    Elf32_Word nbucket = dyn->hashtab[0];
    Elf32_Word* chaintab = &dyn->hashtab[nbucket + BUCKET_OFFSET];
    int bucket_id = hash % nbucket;
    Elf32_Word* chain = &dyn->hashtab[bucket_id + BUCKET_OFFSET];
    while (*chain != STN_UNDEF) {
        char* node_name = &dyn->strtab[dyn->symtab[*chain].st_name];
        if (strcmp(node_name, name) == 0) {
            return mem + dyn->symtab[*chain].st_value;
        }
        chain = &chaintab[*chain];
    }
    return NULL;
}
