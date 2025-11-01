#include <string.h>
#include <stdlib.h>

#include "string_parse.h"

char * replaceWord(char * string, unsigned long str_total_space, char * old_word, char * new_word)
{
  //  replaces a word in a string. returns NULL if it fails

  char * old_word_loc = findWordInString(string, old_word);

  if (!old_word_loc)
  {
    return NULL;
  }

  unsigned long str_len = strlen(string);
  unsigned long old_word_len = strlen(old_word);
  unsigned long new_word_len = strlen(new_word); 
  unsigned long new_len = str_len - old_word_len + new_word_len;

  if (new_len > str_total_space)
  {
    return NULL;
  }

  char * str_buf = calloc(str_total_space, sizeof(char));
  
  strcpy(str_buf, old_word_loc + old_word_len);
  strcpy(old_word_loc, new_word);
  strcpy(old_word_loc + new_word_len, str_buf);
  
  free(str_buf);

  return string;
}

