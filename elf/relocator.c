#include "./relocator.h"

#include <elf.h>

void apply_relocations(void* mem, struct dyninfo* dyn, void *(*getsym)(const char *name)) {
    int num_rel = dyn->rel_size / sizeof(Elf32_Rel);
    printf("Applying relocations. Mem starts at 0x%x\n", mem);
    for (int i=0; i<num_rel; i++) {
        printf("%x\n", dyn->reltab[i].r_offset);
        Elf32_Addr addend = *(Elf32_Addr*)&mem[dyn->reltab[i].r_offset];
        int rel_type = ELF32_R_TYPE(dyn->reltab[i].r_info);
        int rel_sym = ELF32_R_SYM(dyn->reltab[i].r_info);
        char* name = &dyn->strtab[dyn->symtab[rel_sym].st_name];
        Elf32_Addr sym_value = dyn->symtab[rel_sym].st_value;
        void* destination;
        if (sym_value == STN_UNDEF) {
            destination = getsym(name);
        } else {
            destination = &mem[sym_value];
        }
        printf("Symbol %s resolved to addr 0x%x\n", name, destination);
        Elf32_Addr rel;
        switch(rel_type) {
            /*
            A = addend used to compute the value of relocatable field
            S = value of symbol whose index resides in the relocation entry
            P = Section offset or address of the storage unit being relocated, computed using r_offset
            B = The base address at which a shared object is loaded into memory during execution. Generally, a shared object file is built with a base virtual address of 0. However, the execution address of the shared object is different. See Program Header.
            */
            case R_386_32: // S + A
                printf("Relocation R_386_32\n");
                rel = &destination[addend];
            break;
            case R_386_PC32: // S + A - P
                printf("Relocation R_386_PC32\n");
                rel = destination + addend - mem - dyn->reltab[i].r_offset;
            break;
            case R_386_JMP_SLOT: // S
                printf("Relocation R_386_JMP_SLOT\n");
                rel = destination;
            break;
            case R_386_GLOB_DAT: // S
                printf("Relocation R_386_GLOB_DAT\n");
                rel = destination;
            break;
            case R_386_RELATIVE: // B + A
                printf("Relocation R_386_RELATIVE of 0x%x relo to 0x%x\n", addend, mem + addend);
                rel = &mem[addend];
            break;
        }
        printf("Relocating at address 0x%x \n", &mem[dyn->reltab[i].r_offset]);
        memcpy(&mem[dyn->reltab[i].r_offset], &rel, sizeof(Elf32_Addr));
    }
}