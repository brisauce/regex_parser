#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "error_handling.h"
enum error_state error_state = NO_ERROR_STATE;

//  This array should have a number of elements equal to the last defined enum for enum error_state,
//  plus one, because enum error state starts counting from zero.
char * error_msg[FAILED_FILE_WRITE + 1] = {
  "No error state.",
  "Invalid escape character.",
  "Expected closing brace in character class",
  "Word not found.",
  "Invalid character class.",
  "Missing argument in \"Match one or more chars\" regex.",
  "Missing argument in \"Match zero or one char\" regex.",

  "Failed to add data to dynamic array.",

  "Failed to allocate memory.",
  "Failed to open file.",
  "Failed to write to file."

};

void setErrorState(enum error_state state)
{
  error_state = state;
}

enum error_state getCurrentErrorState(void)
{
  return error_state;
}

void printErrorState(char * errmsg)
{ 
  //  Pass a string to be concatenated with a description of the error state 
  //  which is then printed.
  //
  //  If no string (NULL) is passed, then the error message for the current error state is printed
  //  by itself.

  char * desc = error_msg[error_state];

  if (!errmsg)
  {
    puts(desc);
    return;
  }

  unsigned long errmsg_len = strlen(errmsg);

  unsigned int addtl_space = 1;

  if (errmsg[errmsg_len - 1] != ' ')
  {
    addtl_space++;
  }

  unsigned long desc_len = strlen(desc);

  //  Adding 1 (addtl_space) to the value to make room for \0, as 
  //
  //  strlen(string1) + strlen(string2)
  //
  //  does not seem to include this on its own.
  //
  //  Also, if there isn't a space added to the end of the error message, one will be added
  //  for readability, or else there won't be a space between errmsg and desc.
  //  In this case, addtl_space will be 2 instead of 1.

  unsigned int final_size = (desc_len + errmsg_len + addtl_space) * sizeof(char); 
  char * final_msg = malloc(final_size);

  strcpy(final_msg, errmsg);

  if (addtl_space != 1)
  {
    strcat(final_msg, " ");
  }

  strcat(final_msg, desc);

  puts(final_msg);

  free(final_msg);
}
