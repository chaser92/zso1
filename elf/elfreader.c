// Copyright 1 2

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "./elfreader.h"

int load_elf(const char* filename, struct elfinfo* elf) {
    printf("load_elf\n");
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        return fd;
    }
    // TODO(c) error check the mmap
    elf->raw = mmap(NULL, fd_length(fd), PROT_READ, MAP_PRIVATE, fd, 0);
    elf->header = (Elf32_Ehdr*)elf->raw;
    elf->sht = (Elf32_Shdr*)(elf->raw + elf->header->e_shoff);
    elf->pht = (Elf32_Phdr*)(elf->raw + elf->header->e_phoff);
    elf->sht_len = elf->header->e_shnum;
    elf->pht_len = elf->header->e_phnum;
    printf("sht %x\n", elf->sht);
    return 0;
}

int determine_program_size(struct elfinfo* elf) {
    printf("determine_program_size\n");
    int max = 0;
    for (int i = 0; i < elf->pht_len; i++) {
        if (elf->pht[i].p_type == PT_LOAD) {
            if (elf->pht[i].p_vaddr + elf->pht[i].p_memsz > max) {
                max = elf->pht[i].p_vaddr + elf->pht[i].p_memsz;
            }
        }
    }
    return max;
}

void* alloc_memory(struct elfinfo* elf) {
    printf("alloc\n");
    int size = determine_program_size(elf);
    void* mem = valloc(size);
    if (mprotect(mem, size, (PROT_READ | PROT_WRITE |PROT_EXEC))) {
        exit(-1);
    }
    return mem;
}

void load_segments(void* mem_start, struct elfinfo* elf) {
    printf("load_segments\n");
    for (int i = 0; i < elf->pht_len; i++) {
        if (elf->pht[i].p_type == PT_LOAD) {
            /*printf("%lld\n", elf->pht[i].p_offset);
            printf("%lld\n", elf->pht[i].p_vaddr);
            printf("%lld\n", elf->pht[i].p_filesz);
            printf("%lld\n", elf->pht[i].p_memsz);*/
            memcpy(mem_start + elf->pht[i].p_vaddr,
                elf->raw + elf->pht[i].p_offset, elf->pht[i].p_filesz);
        }
    }
}

struct dyninfo load_dynamic(struct elfinfo* elf) {
    printf("load_dynamic\n");

    struct dyninfo dyn;
    dyn.elf = elf;
    dyn.rel_size = 0;
    for (int i = 0; i < elf->sht_len; i++) {
        if (elf->sht[i].sh_type == SHT_DYNAMIC) {
            printf("SHT_dynamic %d\n", 0);
            int numDyns = elf->sht[i].sh_size / elf->sht[i].sh_entsize;
            Elf32_Dyn* dyns = (Elf32_Dyn*)(elf->raw + elf->sht[i].sh_offset);
            for (int i = 0; i < numDyns; i++) {
                printf("dtag%d\n", dyns[i].d_tag);
                switch (dyns[i].d_tag) {
                    case DT_STRTAB:
                        printf("You are in a dark room. You see DT_STRTAB @ . 0x%x\n", dyns[i].d_un.d_ptr);
                        dyn.strtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_SYMTAB:
                        dyn.symtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_REL:
                        printf("You are in a dark room. You see DT_REL. What do you do?\n");
                        dyn.reltab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_RELSZ:
                        printf("Found a few grams of DT_RELSZ!\n");
                        dyn.rel_size = dyns[i].d_un.d_val;
                        break;
                    case DT_HASH:
                        printf("Found a few grams of hash! at 0x%x \n", dyns[i].d_un.d_ptr);
                        dyn.hashtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                }
            }
        }
    }

    return dyn;
}

int fd_length(int fd) {
    struct stat buf;
    fstat(fd, &buf);
    printf("Program size is %d\n", buf.st_size);
    return buf.st_size;
}
