// Copyright 1 2
#include <stdlib.h>

#include "./elfreader.h"
#include "./elfhash.h"
#include "./relocator.h"

struct library {
    struct elfinfo elf;
    struct dyninfo dyn;
    void* mem;
};

struct library* library_load (const char *name, void *(*getsym)(const char *name)) {
    printf("library_load %s\n", name);
    struct library* lib = (struct library*)malloc(sizeof(struct library));
    load_elf(name, &lib->elf);
    lib->mem = alloc_memory(&lib->elf);
    load_segments(lib->mem, &lib->elf);
    lib->dyn = load_dynamic(&lib->elf);
    apply_relocations(lib->mem, &lib->dyn, getsym);
    return lib;
}

void *library_getsym(struct library *lib, const char *name) {
    return resolve_symbol(lib->mem, &lib->dyn, name);
}
