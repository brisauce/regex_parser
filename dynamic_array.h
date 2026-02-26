#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include <stdbool.h>
#include <stdlib.h>

bool dynArrayInit (unsigned int num_items, size_t item_size);
bool dynArrayAdd (void * item);
void * dynArrayRead (size_t * element_size);
void dynArrayDestroy(void);
unsigned int dynArrayGetIndex (void);
unsigned int dynArrayGetArraySize (void);
void * dynArrayGetData (unsigned int index);
unsigned int dynArrayMoveToIndex(unsigned int index) __attribute_warn_unused_result__;


typedef enum {
  DAMOVE_FAIL,
  DAMOVE_SUCCEED
}dynArrayMove;

#endif
