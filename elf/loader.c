// Copyright 1 2
#include "./elfreader.h"
#include "./elfhash.h"

struct library {
    struct elfinfo elf;
    struct dyninfo dyn;
};

struct library* library_load (const char *name, void *(*getsym)(const char *name)) {
    struct library* lib = (struct library*)malloc(sizeof(struct library));
    load_elf("/usr/lib/libgettextlib-0.18.3.so", &lib->elf);
    void* mem = alloc_memory(&lib->elf);
    load_segments(mem, &lib->elf);
    lib->dyn = load_dynamic(&lib->elf);
    return lib;
}

void *library_getsym(struct library *lib, const char *name) {
    return resolve_symbol(&lib->dyn, name);
}
