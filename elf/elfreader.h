// Copyright 1 2
#ifndef __ELFREADER_H
#define __ELFREADER_H

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
    Elf32_Rel* reltab;
    Elf32_Rel* jmpreltab;
    Elf32_Word* hashtab;
    int rel_size;
    int jmprel_size;
    int rel_entry;
};

int load_elf(const char* fn, struct elfinfo* target);
int determine_program_size(struct elfinfo*);
struct dyninfo load_dynamic(struct elfinfo* elf);

#endif
