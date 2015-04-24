#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "./err.h"
#include "./elfreader.h"

int fd_length(int fd) {
    struct stat buf;
    if (fstat(fd, &buf) < 0) {
        syserr("fstat");
    };
    return buf.st_size;
}

void load_elf(const char* filename, struct elfinfo* elf) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        syserr("load_elf");
    }
    // TODO(c) error check the mmap
    elf->raw = mmap(NULL, fd_length(fd), PROT_READ, MAP_PRIVATE, fd, 0);
    if (elf->raw == MAP_FAILED) {
        syserr("mmap");
    }
    elf->header = (Elf32_Ehdr*)elf->raw;
    elf->sht = (Elf32_Shdr*)(elf->raw + elf->header->e_shoff);
    elf->pht = (Elf32_Phdr*)(elf->raw + elf->header->e_phoff);
    elf->sht_len = elf->header->e_shnum;
    elf->pht_len = elf->header->e_phnum;
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

struct dyninfo load_dynamic(struct elfinfo* elf) {
    struct dyninfo dyn;
    dyn.elf = elf;
    dyn.rel_size = 0;
    dyn.jmprel_size = 0;
    for (int i = 0; i < elf->sht_len; i++) {
        if (elf->sht[i].sh_type == SHT_DYNAMIC) {
            int numDyns = elf->sht[i].sh_size / elf->sht[i].sh_entsize;
            Elf32_Dyn* dyns = (Elf32_Dyn*)(elf->raw + elf->sht[i].sh_offset);
            for (int i = 0; i < numDyns; i++) {
                switch (dyns[i].d_tag) {
                    case DT_STRTAB:
                        dyn.strtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_SYMTAB:
                        dyn.symtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_REL:
                        dyn.reltab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_RELSZ:
                        dyn.rel_size = dyns[i].d_un.d_val;
                        break;
                    case DT_JMPREL:
                        dyn.jmpreltab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                    case DT_PLTRELSZ:
                        dyn.jmprel_size = dyns[i].d_un.d_val;
                        break;
                    case DT_HASH:
                        dyn.hashtab = elf->raw + dyns[i].d_un.d_ptr;
                        break;
                }
            }
        }
    }
    return dyn;
}
