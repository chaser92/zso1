// Copyright 1 2
#ifndef __ELF_H
#define __ELF_H

#include <elf.h>

struct elfinfo {
    void* raw;
    Elf32_Ehdr* header;
    Elf32_Shdr* sht;
    Elf32_Phdr* pht;
    int sht_len;
    int pht_len;
};

struct dyninfo {
    struct elfinfo* elf;
    char* strtab;
    Elf32_Sym* symtab;
    Elf32_Rel* rel;
    Elf32_Word* hashtab;
    int rel_size;
    int rel_entry;
};

int fd_length(int fd);
int load_elf(const char* fn, struct elfinfo* target);
int determine_program_size(struct elfinfo*);
void load_segments(void* mem_start, struct elfinfo* elf);
struct dyninfo load_dynamic(struct elfinfo* elf);
void* alloc_memory(struct elfinfo* elf);

#endif
