#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include <stdbool.h>
#include <stdlib.h>

void * dynArrayInit(size_t num_items, size_t item_size)__attribute_warn_unused_result__;
void dynArrayAdd(void ** array_ptr, void * item);
void dynArrayDestroy (void ** array);
size_t dynArrayGetArraySize(void * array);
#endif
