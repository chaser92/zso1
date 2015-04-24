#include "./allocator.h"

#include <sys/mman.h>
#include <malloc.h>
#include <string.h>

#include "./err.h"

void* alloc_memory(struct elfinfo* elf) {
    int size = determine_program_size(elf);
    void* mem = valloc(size);
    if (mem == NULL) {
        syserr("valloc");
    }
    return mem;
}

void load_segments(void* mem_start, struct elfinfo* elf) {
    for (int i = 0; i < elf->pht_len; i++) {
        if (elf->pht[i].p_type == PT_LOAD) {
            int pread = (elf->pht[i].p_flags & PF_R);
            int pwrite = (elf->pht[i].p_flags & PF_W);
            memset(mem_start + elf->pht[i].p_vaddr, 0, elf->pht[i].p_memsz);
            memcpy(mem_start + elf->pht[i].p_vaddr,
                elf->raw + elf->pht[i].p_offset, elf->pht[i].p_filesz);
            mprotect(mem_start, elf->pht[i].p_memsz,
                    (pread ? PROT_READ : 0) |
                    (pwrite ? PROT_WRITE : 0) |
                    PROT_EXEC);
        }
    }
}
