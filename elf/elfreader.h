// Copyright 1 2
#ifndef __ELF_H
#define __ELF_H

#include <elf.h>

struct elfinfo {
    void* raw;
    Elf64_Ehdr* header;
    Elf64_Shdr* sht;
    Elf64_Phdr* pht;
    int sht_len;
    int pht_len;
};

struct dyninfo {
    struct elfinfo* elf;
    char* strtab;
    Elf64_Sym* symtab;
    Elf64_Rel* rel;
    Elf64_Word* hashtab;
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
