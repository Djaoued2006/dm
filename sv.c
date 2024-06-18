#include "./sv.h"

View sv_init(char *content) {
    return (View) {
        .content = content, 
        .current = 0,
        .size = strlen(content),
    };
}

char *sv_to_cstr(View *view) {
    if (view->size == 0) return NULL;
    char *str = (char *)malloc(sizeof(char) * (view->size + 1));
    assert(str != NULL && "ERROR: 'sv_to_cstr', failed to malloc the string\n");
    memcpy(str, view->content, view->size);
    str[view->size] = '\0';
    return str;
}


void sv_skip_spaces(View *view) {
    while (view->current != view->size) {
        char ch = view->content[view->current];
        if (ch == ' ' || ch == '\t') view->current++;
        else break;
    }
}
