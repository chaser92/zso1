// Copyright 1 2

#ifndef __RELOCATOR_H
#define __RELOCATOR_H

#include "./elfreader.h"

void apply_relocations(void* mem, struct dyninfo* dyn, void *(*getsym)(const char *name));

#endif
