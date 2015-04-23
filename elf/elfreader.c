// Copyright 1 2

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "./elfreader.h"

int load_elf(const char* filename, struct elfinfo* elf) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        return fd;
    }
    // TODO(c) error check the mmap
    elf->raw = mmap(NULL, fd_length(fd), PROT_READ, MAP_PRIVATE, fd, 0);
    elf->header = (Elf64_Ehdr*)elf->raw;
    elf->sht = (Elf64_Shdr*)(elf->raw + elf->header->e_shoff);
    elf->pht = (Elf64_Phdr*)(elf->raw + elf->header->e_phoff);
    elf->sht_len = elf->header->e_shnum;
    elf->pht_len = elf->header->e_phnum;
    return 0;
}

int determine_program_size(struct elfinfo* elf) {
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
    int size = determine_program_size(elf);
    return malloc(size);
}

void load_segments(void* mem_start, struct elfinfo* elf) {
    for (int i = 0; i < elf->pht_len; i++) {
        if (elf->pht[i].p_type == PT_LOAD) {
            printf("%lld\n", elf->pht[i].p_offset);
            printf("%lld\n", elf->pht[i].p_vaddr);
            printf("%lld\n", elf->pht[i].p_filesz);
            printf("%lld\n", elf->pht[i].p_memsz);
            memcpy(mem_start + elf->pht[i].p_vaddr,
                elf->raw + elf->pht[i].p_offset, elf->pht[i].p_filesz);
        }
    }
}

struct dyninfo load_dynamic(struct elfinfo* elf) {
    struct dyninfo dyn;
    for (int i = 0; i < elf->sht_len; i++) {
        if (elf->sht[i].sh_type == SHT_DYNAMIC) {
            int numDyns = elf->sht[i].sh_size / elf->sht[i].sh_entsize;
            Elf64_Dyn* dyns = (Elf64_Dyn*)(elf->raw + elf->sht[i].sh_offset);
            for (int i = 0; i < numDyns; i++) {
                printf("dtag%d\n", dyns[i].d_tag);
                switch (dyns[i].d_tag) {
                    case DT_STRTAB:
                        printf("strtab\n");
                        dyn.strtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_SYMTAB:
                        dyn.symtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_REL:
                        printf("You are in a dark room. You see DT_REL. What do you do?\n");
                        dyn.rel = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_RELSZ:
                        printf("Found a few grams of DT_RELSZ!\n");
                        dyn.rel_size = dyns[i].d_un.d_val;
                        break;
                    case DT_HASH:
                        printf("Found a few grams of hash!\n");
                        dyn.hashtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                }
            }
        }
    }
}

void apply_relocations(void* mem, struct dyninfo* dyn) {
    // TODO(chaser)
}

int fd_length(int fd) {
    struct stat buf;
    fstat(fd, &buf);
    return buf.st_size;
}
