#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "arena.h"
#include "string_parse.h"
#include "regexp.h"
#include "error_handling.h"
#include "word_loc_struct.h"
#include "file_io.h"

char * findWordInString(char * string, char * word)
{
  for (unsigned int i = 0 ; string[i]; i++)
  {
    if (string[i] == word[0])
    {
      if (!strncmp(string + i, word, strlen(word)))
      {
        return string + i;
      }
    }
  }
  setErrorState(WORD_NOT_FOUND);
  return NULL;
}

//  A note on why 
//
//  ftell(a->fp) - sizeof(char)l 
//
//  Appears in the code. 
//
//  fgetc, which is the function used to grab a single character from the file, also 
//  advances the position indicator by one character width in the file. This makes ftell(),
//  which returns the location of the position indicator in the file,
//  point to the character *following* the character which I want to know the position of.
//  Useful for conciseness, but not so much for my specific purposes

void noRegex(arena * a, char ** word_ptr, long * word_start_in_string)
{
  if (fgetc(a->fp) == **word_ptr)
  {

    //  Match. If it's the first letter in the word, save it in word_start_in_string.
    if (*word_ptr == a->word)
    {
      *word_start_in_string = ftell(a->fp) - (long) sizeof(char);
    }

    (*word_ptr)++;
  }
  else if (*word_ptr != a->word)
  {
    //  No match and need to reset word_ptr to start looking from the beginning again
    *word_ptr = a->word;
  }

}

void charPattern (arena * a, char ** word_ptr, long * word_start_in_string)
{
  char A;
  char Z;
  char * new_ptr = parseCharPattern(*word_ptr, &A, &Z);
  if (!A || !Z)
  {
    //  Error in parsing or invalid char class
    return;
  }

  if (!isInCharClass(fgetc(a->fp), A, Z))
  {
    //  String doesn't match word, start looking for the word again from the start
    *word_ptr = a->word;
  }
  else 
  {
    //  Match found, earmark pointer if at the start of the word
    if (*word_ptr == a->word)
    {
      *word_start_in_string = ftell(a->fp) - (long) sizeof(char);
    }

    //  parseCharPattern will have advanced the word pointer up to the ']' part in the char 
    //  pattern regexp, this will move it past that so we are completely past it
    *word_ptr = new_ptr + 1;
  }
}

void matchOneOrMoreChars(arena * a, char ** word_ptr, long * word_start_in_string)
{
  //  The first two chars in the word will be the argument, i.e. what character we are looking for,
  //  followed by the regex character.

  long start = ftell(a->fp);
  unsigned int length = 0;

  #define MATCH_NOT_FOUND LONG_MAX
  long last_matching_char = MATCH_NOT_FOUND;
  if (validEscChar(*word_ptr, &length))
  {
    //  Length of the regex includes the esc. char. regex and the '*' for "one or more chars"
    //  The length of the esc. char will be accounted for from validEscChar()
    length ++;
    while(true)
    {
      char c = fgetc(a->fp);

      if (c == EOF)
      {
        break;
      }
      else if (c != (*word_ptr)[1])
      {
        break;
      }

      last_matching_char = ftell(a->fp);
    }
  }
  else if (validCharClass(*word_ptr, &length))
  {
    //  The "one or more chars" regex can be preceded by a char. class regex and grouped
    //  together to search for a series of characters.

    //  Length of the regex includes the char. class regex and the '*' for "one or more chars",
    //  but "length" will only take the word_ptr to the terminating "]" in the char class regex.
    //  To account for both of these, we must add 2 to move the pointer past the "]" and "*".

    length += 2;

    char A;
    char Z;
    parseCharPattern(*word_ptr, &A, &Z);

    while(true)
    {
      char c = fgetc(a->fp);

      if (c == EOF)
      {
        break;
      }
      else if (!isInCharClass(c, A, Z))
      {
        break;
      }

      last_matching_char = ftell(a->fp);
    }
  }
  else 
  {
    //  Regular old regex for "one or more chars", no grouping

    length = 2;
    while(true)
    {
      char c = fgetc(a->fp);

      if (c == EOF)
      {
        break;
      }
      else if (c != **word_ptr)
      {
        break;
      }

      last_matching_char = ftell(a->fp);
    }
  }

  if (ftell(a->fp) - (long) sizeof(char) == start)
  {
    //  Did not advance the pos. indicator beyond the first char. checked. 
    //  No match found. Need to start again from the beginning.
    *word_ptr = a->word;
  }
  else 
  {
    if (*word_ptr == a->word)
    {
      *word_start_in_string = start;
    }
    
    if (last_matching_char != MATCH_NOT_FOUND)
    {
      fseek(a->fp, last_matching_char, SEEK_SET);
    }

    *word_ptr += length;
  }
}

void matchZeroOrOneChar(arena * a, char ** word_ptr, long * word_start_in_string)
{
  //  The first two chars in the word will be the argument followed by the regex character

  unsigned int length;
  char cur_file_char = fgetc(a->fp);

  if (validEscChar(*word_ptr, &length))
  {
    if ((*word_ptr)[1] != cur_file_char)
    {
      fseek(a->fp, -sizeof(char), SEEK_CUR);
    }
    else if (*word_ptr == a->word)
    {
      fseek(a->fp, -sizeof(char), SEEK_CUR);
      *word_start_in_string = ftell(a->fp);
    }

    *word_ptr += length + sizeof(char);
  }
  else if (validCharClass(*word_ptr, &length))
  {
    char A;
    char Z;
    parseCharPattern(*word_ptr, &A, &Z);

    if (!isInCharClass(cur_file_char, A, Z))
    {
      fseek(a->fp, -sizeof(char), SEEK_CUR);
    }
    else if (*word_ptr == a->word)
    {
      fseek(a->fp, -sizeof(char), SEEK_CUR);
      *word_start_in_string = ftell(a->fp);    
    }

    //  When we are at this point, the word pointer will be at the beginning of the char 
    //  class regex. It has to be moved past the regex, the majority of which will be done by 
    //  adding "length" to it. This will leave it at the terminating right bracket of the regex,
    //  so to move it past that and the "?" we move it 2 spaces.
    *word_ptr += length + (2 * sizeof(char));
  }
  else
  {
    if (**word_ptr != cur_file_char)
    {
      fseek(a->fp, -sizeof(char), SEEK_CUR);
    }
    else if (*word_ptr == a->word)
    {
      fseek(a->fp, -sizeof(char), SEEK_CUR);
      *word_start_in_string = ftell(a->fp);
    }

    *word_ptr += 2 * sizeof(char);
  }
}

void escapeChar (arena * a, char ** word_ptr, long * word_start_in_string)
{
  if (fgetc(a->fp) != *word_ptr[1])
  {
    *word_ptr = a->word;
  }
  else if (*word_ptr == a->word)
  {
    *word_start_in_string = ftell(a->fp) - (long) sizeof(char);
  }
  else 
  {
    *word_ptr += 3;
  }
}

int findWordInStringRegex(arena * a, word_loc * loc)
{
  /*
   *  Parses a string for a char sequence which matches a regex containing word.
   *
   *  Returns -1 (STRING_PARSE_FAIL) if this fails due to a passed regex word which has poorly 
   *  implemented regex.
   *
   *  Otherwise, returns a positive number.
   */

  long file_start_pos = ftell(a->fp);
  char first_char = fgetc(a->fp);

  if (first_char == '*')
  {
    setErrorState(MATCH_ANY_NUM_MISSING_ARG);
    return STRING_PARSE_FAIL;
  }

  if (first_char == '?')
  {
    setErrorState(MATCH_ONE_CHAR_MISSING_ARG);
    return STRING_PARSE_FAIL;
  }

  //  move the file position indicator back to where it was when the function was called
  fseek(a->fp, file_start_pos, SEEK_SET);

  //  Optimization- allows the regex parse function to only run when the word pointer changes
  char * current_word_index = NULL;

  //  The state of which regex is currently being looked at in the word
  enum whichRegex state;

  //  Remembers where the current word index is
  char * word_ptr = a->word;

  //  Tracks the location of the identified word and returns its location to the user.
  //  Data is formatted as the file position indicator returned by ftell()
  long word_start_in_string = NOT_FOUND;
  long word_end_in_string = NOT_FOUND;

  //  Used to move past the regexp which was just parsed in the word.
  unsigned int length;

  while (!feof(a->fp) && *word_ptr)
  {

    //  Step through the file, searching for the word. 
    //  Once the first character in the word is detected, start moving through the word, checking 
    //  matches character by character. The word pointer will only be advanced if there is a match.
    //
    //  If any letter past the first letter does not match, stop parsing the word and move the pointer 
    //  for the word back to the start of the word. Start searching for the first letter in the word again. 
    //
    //  if the regex word is formatted incorrectly, return indicating an error.
    //
    //  Do this until either the end of the file or the word is reached.

    if (current_word_index != word_ptr)
    {
      //  Optimization - This prevents the program from needing 
      //  to check the word for every iteration of the loop
      state = regexpParse(word_ptr, &length);
      current_word_index = word_ptr;
    }



    switch (state) 
    {
    case NONE:
      noRegex(a, &word_ptr, &word_start_in_string);
      break;
    case CHAR_PATTERN:
      charPattern(a, &word_ptr, &word_start_in_string);
      break;
    case MATCH_ONE_OR_MORE_CHARS: 
      matchOneOrMoreChars(a, &word_ptr, &word_start_in_string);
      break;
    case MATCH_ZERO_OR_ONE_CHAR:
      matchZeroOrOneChar(a, &word_ptr, &word_start_in_string);
      break;
    case ESCAPE_CHAR:
      escapeChar(a, &word_ptr, &word_start_in_string);
      break;
    case ERROR:
      return STRING_PARSE_FAIL;
    }

    if (getCurrentErrorState() != NO_ERROR_STATE)
    {
      return STRING_PARSE_FAIL;
    }
  }

  if (a->word + strlen(a->word) == word_ptr)
  {
    //  Reached the end of the regex word, meaning there was a complete match.
    word_end_in_string = ftell(a->fp) - (long) sizeof(char);
  }

  loc->word_start_pos = word_start_in_string;
  loc->word_end_pos = word_end_in_string;


  return STRING_PARSE_SUCCESS;
}
