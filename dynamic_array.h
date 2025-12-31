#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include <stdbool.h>
#include <stdlib.h>

bool dynArrayInit (unsigned int num_items, size_t item_size);
bool dynArrayAdd (void * item);
void * dynArrayRead (size_t * element_size);
void dynArrayDestroy(void);


#endif
