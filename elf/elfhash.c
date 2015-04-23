// Copyright 1 2

#include "./elfreader.h"

#define BUCKET_OFFSET 2

unsigned long elf_hash(const unsigned char *name) {
    unsigned long h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        if (g = h & 0xf0000000) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h;
}

void* resolve_symbol(struct dyninfo* dyn, const char *name) {
    unsigned int hash = elf_hash(name);
    Elf64_Word nbucket = dyn->hashtab[0];

    int bucket_id = hash % nbucket;
    printf("nbucket%d\n", bucket_id);
    Elf64_Word* chain = &dyn->hashtab[bucket_id + BUCKET_OFFSET];
    while (*chain != STN_UNDEF) {
        printf("chain%d\n", *chain);
        char* node_name = dyn->strtab[dyn->symtab[*chain].st_name];
        printf("%s\n", node_name);
        if (strcmp(node_name, name) == 0) {
            return dyn->symtab[*chain].st_value;
        }
        chain++;
    }
}
