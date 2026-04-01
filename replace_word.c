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

enum copyFileStatus {
  COPYFILE_SUCCESS,
  COPYFILE_ERROR
};

int copyFile(FILE * src, long src_ptr, FILE * dest, long dest_ptr)
{
  //  Copies a file from the position of the position indicator forward until the end of the file 
  //  into the second file argument at its current position.
  char buffer[100];
  size_t bytes;

  if (fseek(src, src_ptr, SEEK_SET))
  {
    return COPYFILE_ERROR;
  }
  
  if (fseek(dest, dest_ptr, SEEK_SET))
  {
    return COPYFILE_ERROR;
  }

  while ((bytes = fread(buffer, sizeof(char), sizeof(buffer), src)) > 0)
  {
    if (fwrite(buffer, sizeof(char), bytes, dest) != bytes)
    {
      return COPYFILE_ERROR;
    }
  }

  fflush(dest);

  return COPYFILE_SUCCESS;
}

int replaceWordinFile(arena * a, word_loc loc)
{
  //  Go to the point in the file where the end of the old word is located, copy the old file contents 
  //  into a new temporary file which is destroyed by the end of the function, paste 
  //  the new word into the old file, then copy the old file contents to the end of the file

  char * temp_file_name = "temp"; 
  FILE * temp_file = fopen(temp_file_name, "w+");

  if (!temp_file)
  {
    setErrorState(FAILED_FILE_OPEN);
    return REPLACE_WORD_ERROR;
  }

  //  Go one char-width beyond the end of the word in the file. We don't want to get the end 
  //  of the original word copied into the temp file.

  if (copyFile(a->fp, loc.word_end_pos + sizeof(char), temp_file, 0 /* Start of file*/) == COPYFILE_ERROR)
  {
    //  error occured writing to file
    setErrorState(FAILED_FILE_WRITE);
    return REPLACE_WORD_ERROR;
  }

  //  Go to where the original word started and write the new word in. The position 
  //  indicator for a->fp should be at the end of the new word now and we can overwrite 
  //  the rest of the file.
  fseek(a->fp, loc.word_start_pos, SEEK_SET);

  if (fputs(a->new_word, a->fp) == EOF)
  {
    setErrorState(FAILED_FILE_WRITE);
    return REPLACE_WORD_ERROR;
  }

  fflush(a->fp);

  //  Go to the start of the temp file so we can rewrite everything back to the 
  //  original file.
  fseek(a->fp, loc.word_start_pos + strlen(a->new_word), SEEK_SET);

  if (copyFile(temp_file, 0 /* Start of file */, a->fp, ftell(a->fp)) == COPYFILE_ERROR)
  {
    setErrorState(FAILED_FILE_WRITE);
    return REPLACE_WORD_ERROR;
  }


  fclose(temp_file);
  remove(temp_file_name);

  return REPLACE_WORD_SUCCESS;
}

