#ifndef ASM_H
#define ASM_H

#include "./lexer.h"
#include "../machine/dm.h"


void dm_assemble(Machine *dm, const char *filename);
void free_lines(Lines lines);

#endif