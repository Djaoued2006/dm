#include "./da.h"

DYN_ARR arr_init(size_t item_size) {
    DYN_ARR arr;
    arr.size = DYN_ARR_INITIAL_SIZE;
    arr.count = 0;
    arr.item_size = item_size;
    arr.items = (void **)malloc(arr.size * arr.item_size);
    return arr;
}

static int isfull(DYN_ARR *arr) {
    return (arr->size == arr->count);
}

void arr_resize(DYN_ARR *arr) {
    arr->size *= 2;
    arr->items = (void **)realloc(arr->items, arr->size * arr->item_size);
    if (arr->items == NULL) 
        ARR_CRASH(arr, "Couldn't resize the array, Old size: %zu, New size: %zu\n", arr->count, arr->size);
}

void arr_get_item(DYN_ARR *arr, void *dest, size_t index) {
    if (index > arr->count) {
        ARR_CRASH(arr, "Index out of range, Size: %zu, Index: %zu", arr->count, index);
    }

    void *source = arr->items + index * arr->item_size;
    memcpy(dest, source, arr->item_size);
} 

void arr_set_item(DYN_ARR *arr, void *source, size_t index) {
    if (index > arr->count) {
        ARR_CRASH(arr, "Index out of range, Size: %zu, Index: %zu", arr->count, index);
    }

    void *dest = arr->items + index * arr->item_size;
    memcpy(dest, source, arr->item_size);
}

void arr_append(DYN_ARR *arr, void *source) {
    if (isfull(arr))
        arr_resize(arr);
    
    arr_set_item(arr, source, arr->count);
    arr->count++;
}


void arr_clean(DYN_ARR *arr) {
    free(arr->items);
}

