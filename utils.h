#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARR_SIZE(xs) sizeof(xs)/sizeof((xs)[0])

#define NEW_LINE '\n'
#define WHITE_SPACE ' '
#define COMMA ','
#define COLON ':'
#define QUOTES '"'


char *fcontent(const char *filename);
int isequal(char *str1, char *str2);

static char *inst_set[] = {
    "push",
    "dup",
    "add", 
    "sub",
    "mul",
    "div",
    "jmp",
    "halt",
    "cmp",
    "je",
    "jne",
    "syscall",
    "push_str",
    "dec",
    "inc",
    "pop",    
    "jl",
    "jg",
    "jle",
    "jge",
};

#endif