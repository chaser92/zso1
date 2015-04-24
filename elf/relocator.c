#include "./relocator.h"

#include <string.h>
#include <elf.h>

void do_apply_relocations(void* mem, struct dyninfo* dyn, Elf32_Rel* reltab, int rel_size, void *(*getsym)(const char *name)) {
    int num_rel = rel_size / sizeof(Elf32_Rel);
    for (int i = 0; i < num_rel; i++) {
        Elf32_Addr addend = *(Elf32_Addr*)(mem + reltab[i].r_offset);
        void* rel;
        int rel_type = ELF32_R_TYPE(reltab[i].r_info);
        int rel_sym = ELF32_R_SYM(reltab[i].r_info);
        char* name = &dyn->strtab[dyn->symtab[rel_sym].st_name];
        Elf32_Addr sym_value = dyn->symtab[rel_sym].st_value;
        void* destination;
        if (sym_value == STN_UNDEF) {
            destination = getsym(name);
        } else {
            destination = mem + sym_value;
        }
        switch (rel_type) {
            case R_386_32:  // S + A
                rel = destination + addend;
            break;
            case R_386_PC32:  // S + A - P
                rel = (void*)(destination + addend - mem - reltab[i].r_offset);
            break;
            case R_386_JMP_SLOT:  // S
                rel = destination;
            break;
            case R_386_GLOB_DAT:  // S
                rel = destination;
            break;
            case R_386_RELATIVE:  // B + A
                rel = mem + addend;
            break;
        }
        memcpy(mem + reltab[i].r_offset, &rel, sizeof(Elf32_Addr));
    }
}

void apply_relocations(void* mem, struct dyninfo* dyn, void *(*getsym)(const char *name)) {
    do_apply_relocations(mem, dyn, dyn->reltab, dyn->rel_size, getsym);
    do_apply_relocations(mem, dyn, dyn->jmpreltab, dyn->jmprel_size, getsym);
}
