#ifndef DA_H
#define DA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DYN_ARR_INITIAL_SIZE 100

#define ARR_CRASH(arr, ...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        arr_clean(arr); \
        exit(1); \
    } while(0)

typedef struct {
    void **items;
    size_t item_size;
    size_t size;
    size_t count;
} DYN_ARR;


DYN_ARR arr_init(size_t item_size);
void arr_get_item(DYN_ARR *arr, void *dest, size_t index);
void arr_set_item(DYN_ARR *arr, void *source, size_t index);
void arr_append(DYN_ARR *arr, void *source);
void arr_clean(DYN_ARR *arr);


#endif 