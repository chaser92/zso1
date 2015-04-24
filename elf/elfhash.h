#ifndef __ELFHASH_H
#define __ELFHASH_H

void* resolve_symbol(void* mem, struct dyninfo* dyn, const char *name);

#endif
