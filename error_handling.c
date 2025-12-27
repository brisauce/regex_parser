#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "error_handling.h"
enum error_state error_state = NO_ERROR_STATE;

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
  //  pass a string to be concatenated with a description of the error state 
  //  which is then printed

  unsigned long errmsg_len = strlen(errmsg);
  
  char * desc;

  switch (error_state)
  {
  case NO_ERROR_STATE:
    desc = "No error state.";
    break;
  case INVALID_ESCAPE_CHAR:
    desc = "Invalid escape character.";
    break;
  case EXPECTED_CLOSING_BRACE:
    desc = "Expected closing brace.";
    break;
  case WORD_NOT_FOUND:
    desc = "Word not found.";
    break;
  case INVALID_CHAR_CLASS:
    desc = "Invalid character class.";
    break;
  case MATCH_ANY_NUM_MISSING_ARG:
    desc = "No argument present for \"*\" regex.";
    break;
  case MATCH_ONE_CHAR_MISSING_ARG:
    desc = "No argument present for \"?\" regex.";
    break;
  /**/
  /*default:*/
  /*  desc = "Unanticipated error state! This is a bug!";*/
  }

  unsigned int addtl_space = 1;

  if (errmsg[errmsg_len - 1] != ' ')
  {
    //  if the last 
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
