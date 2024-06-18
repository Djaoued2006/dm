#include "./utils.h"

size_t fsize(FILE *file) {
    // save current pos;
    fpos_t pos;
    fgetpos(file, &pos);

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);

    fsetpos(file, &pos);
    return size;
}

char *fcontent(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    size_t size = fsize(file);
    char *content = (char *)malloc(sizeof(char) * (size + 1));
    fseek(file, 0, SEEK_SET);

    fread(content, sizeof(char), size, file);
    content[size] = '\0';

    fclose(file);
    return content;
}


int isequal(char *str1, char *str2) {
    return (strcmp(str1, str2) == 0);
}