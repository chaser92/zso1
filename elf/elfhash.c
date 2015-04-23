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

void* resolve_symbol(void* mem, struct dyninfo* dyn, const char *name) {
    unsigned long hash = elf_hash(name);
    Elf32_Word nbucket = dyn->hashtab[0];
    printf("Resolving symbol %s, there are %d buckets\n", name, dyn->hashtab[0]);
    int bucket_id = hash % nbucket;
    printf("Will select bucket %d\n", bucket_id);
    Elf32_Word* chain = &dyn->hashtab[dyn->hashtab[bucket_id + BUCKET_OFFSET + nbucket]];
    while (*chain != STN_UNDEF) {
        printf("Chain %d\n", *chain);
        char* node_name = &dyn->strtab[dyn->symtab[*chain - 1].st_name];
        printf("%s\n", node_name);
        if (strcmp(node_name, name) == 0) {
            printf("I found love @ %x!\n", dyn->elf->raw + dyn->symtab[*chain - 1].st_value);
            return mem + dyn->symtab[*chain - 1].st_value;
        }
        chain++;
    }
}
