#ifndef SV_H
#define SV_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    char *content;
    size_t current;
    size_t size;
} View;

View sv_init(char *content);
char *sv_to_cstr(View *view);
void sv_skip_spaces(View *view);

#endif