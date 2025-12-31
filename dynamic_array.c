#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*  
 *  This implementation of a dynamic array works by tracking both a read and write pointer 
 *  which are used to index an array which can be dynamically resized.
 *
 *  The write pointer will always point to the next unoccupied element in an array and will 
 *  write to it when prompted by dynArrayAdd. 
 *
 *  The read pointer will start at the first element in the list and increment until it 
 *  reaches the write pointer, in which case it will return nothing.
 *
 */

size_t unit_size = 0;
unsigned int w_pointer = 0;
unsigned int r_pointer = 0;
size_t arr_size = 0;
void * array = NULL;

bool dynArrayInit (unsigned int num_items, size_t item_size)
{
  unit_size = item_size;
  arr_size = item_size;
  array = calloc(num_items, item_size);

  return array != NULL;
}

bool dynArrayAdd (void * item)
{
  if (w_pointer == arr_size)
  {
    array = realloc(array, arr_size + arr_size);
    if (!array)
    {
      return false;
    }
  }

  //  This assumes that the size of a unit is made of one or more 8-bit words which are 
  //  boundary aligned
  void * current_element = array + (w_pointer * unit_size);

  for (size_t i = 0; i < unit_size; i++)
  {
    ((uint8_t *) current_element)[i] = ((uint8_t *) item)[i];
  }

  w_pointer ++;

  return true;
}

void * dynArrayRead (size_t * element_size)
{
  //  returns a pointer to the first byte of the item the current read pointer points to.
  //  Will return by reference the size of the item in bytes if a pointer to a size_t 
  //  object is passed to the function, otherwise NULL should be passed.
  //
  

  if (r_pointer >= w_pointer)
  {
    if (element_size)
    {
      *element_size = 0;
    }
    return NULL;
  }

  void * retval = array + (r_pointer * unit_size);
  r_pointer ++;
  
  if (element_size)
  {
    *element_size = unit_size;
  }

  return retval;
}

void dynArrayDestroy(void)
{
  free(array);
}
