#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "string_parse.h"
#include "arena.h"
#include "word_loc_struct.h"
#include "dynamic_array.h"
#include "replace_word.h"
#include "error_handling.h"

enum copyFileStatus copyFile(FILE * src, long src_ptr, FILE * dest, long dest_ptr)
{
  //  Copies a file from the position of the position indicator forward until the end of the file 
  //  into the second file argument at its current position.
  #define BUFFER_SIZE 100
  char buffer[BUFFER_SIZE];
  size_t bytes;

  if (fseek(src, src_ptr, SEEK_SET))
  {
    return COPYFILE_ERROR;
  }
  
  if (fseek(dest, dest_ptr, SEEK_SET))
  {
    return COPYFILE_ERROR;
  }

  while ((bytes = fread(buffer, sizeof(char), BUFFER_SIZE, src)) > 0)
  {
    if (fwrite(buffer, sizeof(char), bytes, dest) != bytes)
    {
      return COPYFILE_ERROR;
    }
  }

  fflush(dest);

  return COPYFILE_SUCCESS;
}
