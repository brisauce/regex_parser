#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "dynamic_array.h"


typedef struct {

  //  size in bytes of the individual elements of the array.
  size_t unit_size;

  //  The next element in the array to write data to. 
  size_t w_pointer;

  //  the total number of indices in the array which can be filled before the 
  //  dynamic array needs to be resized
  size_t capacity;
  
} dynArrayHeader;

void * dynArrayInit(size_t num_items, size_t item_size)
{
  void * retval = calloc(1, sizeof(dynArrayHeader) + (num_items * item_size));
  
  dynArrayHeader * header = retval;

  header->capacity = num_items;
  header->unit_size = item_size;
  header->w_pointer = 0;

  return (dynArrayHeader *) retval + 1;
}

void dynArrayAdd(void ** array_ptr, void * item)
{
  //  Move the pointer to where the header data is, i.e. one header-sized amount of data before
  //  the array
  void * array = *array_ptr;
  dynArrayHeader * header = (dynArrayHeader *) array - 1;

  if (header->w_pointer == header->capacity)
  {
    header->capacity *= 2;
    void * temp = realloc(header, (header->capacity * header->unit_size) + 
                          sizeof(dynArrayHeader)); 
    assert(temp);
    header = temp;
    *array_ptr = (dynArrayHeader *) header + 1;
    array = (dynArrayHeader *) header + 1;
  } 

  void * current_element = array + (header->w_pointer * header->unit_size);

  memcpy(current_element, item, header->unit_size);
  header->w_pointer ++;
}

void dynArrayDestroy (void * array)
{
  free((dynArrayHeader *) array - 1);
}

size_t dynArrayGetArraySize(void * array)
{
  dynArrayHeader * header = (dynArrayHeader *) array - 1;
  return header->w_pointer;
}

