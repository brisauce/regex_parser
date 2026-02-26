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
      state = regexpParse(word_ptr);
      current_word_index = word_ptr;
    }

    //  A note on why 
    //
    //  ftell(a->fp) - 1l 
    //
    //  Appears in the code. 
    //
    //  fgetc, which is the function used to grab a single character from the file, also 
    //  advances the position indicator by one character width in the file. This makes ftell(),
    //  which returns the location of the position indicator in the file,
    //  point to the character *following* the character which I want to know the position of.
    //  Useful for conciseness, but not so much for my specific purposes

    switch (state) 
    {
    case NONE:
      if (fgetc(a->fp) == *word_ptr)
      {
        if (word_ptr == a->word)
        {
          word_start_in_string = ftell(a->fp) - 1l;
        }
        word_ptr++;
      }
      else if (word_ptr == a->word)
      {
        break;
      }
      else 
      {
        word_ptr = a->word;
      }
      break;
    case CHAR_PATTERN:
    {
      char A;
      char Z;
      word_ptr = parseCharPattern(word_ptr, &A, &Z);
      if (!A || !Z)
      {
        //  Error in parsing or invalid char class
        return STRING_PARSE_FAIL;
      }

      if (!isInCharClass(fgetc(a->fp), A, Z))
      {
        //  String doesn't match word, start looking for the word again from the start
        word_ptr = a->word;
      }
      else 
      {
        if (word_ptr == a->word)
        {
          word_start_in_string = ftell(a->fp) - 1l;
        }

        //  parseCharPattern will have advanced the word pointer up to the ']' part in the char 
        //  pattern regexp, this will move it past that so we are completely past it
        word_ptr ++;
      }
      break;
    }
    case MATCH_ONE_OR_MORE_CHARS: 
    {
      //  The first two chars in the word will be the argument, i.e. what character we are looking for,
      //  followed by the regex character.

      long start = ftell(a->fp);
      while(!feof(a->fp) && fgetc(a->fp) == *word_ptr){}

      if (ftell(a->fp) - 1l == start)
      {
        word_ptr = a->word;
      }
      else 
      {
        if (word_ptr == a->word)
        {
          word_start_in_string = ftell(a->fp) - 1;
        }

        word_ptr += 2;
      }

      break;
    }
    case MATCH_ZERO_OR_ONE_CHAR:
      //  The first two chars in the word will be the argument followed by the regex character

      if (*word_ptr != fgetc(a->fp))
      {
        fseek(a->fp, -1, SEEK_CUR);
      }
      else if (word_ptr == a->word)
      {
        word_start_in_string = ftell(a->fp) - 1l;
      }

      word_ptr += 2;
      break;
    case MATCH_ONE_OR_MORE_CHARS_WITH_ESCAPE_CHAR:
    {
      //  The first three characters of the word are all part of a regex- the first is 
      //  the slash indicating an escape character, the second is the character we are looking 
      //  for, and the third is the regex character.

      long start = ftell(a->fp);
      while ( !feof(a->fp) && fgetc(a->fp) == word_ptr[1]){}

      if (ftell(a->fp) - 1l == start)
      {
        word_ptr = a->word;
      }
      else 
      {
        if (word_ptr == a->word)
        {
          word_start_in_string = ftell(a->fp) - 1l;
        }

        word_ptr += 3;
      }

      break;
    }
    case MATCH_ZERO_OR_ONE_CHAR_WITH_ESCAPE_CHAR:

      if (fgetc(a->fp) != word_ptr[1])
      {
        fseek(a->fp, -1, SEEK_CUR);
      }
      else if (word_ptr == a->word)
      {
        word_start_in_string = ftell(a->fp) - 1l;
      }

      word_ptr += 3;

      break;
    case ESCAPE_CHAR:

      if (fgetc(a->fp) != word_ptr[1])
      {
        word_ptr = a->word;
      }
      else if (word_ptr == a->word)
      {
        word_start_in_string = ftell(a->fp) - 1l;
      }
      else 
      {
        word_ptr += 3;
      }

      break;
    case ERROR:
      return STRING_PARSE_FAIL;
    }
  }

  if (a->word + strlen(a->word) == word_ptr)
  {
    //  Reached the end of the regex word, meaning there was a complete match.
    word_end_in_string = ftell(a->fp) - 1l;
  }

  loc->word_start_pos = word_start_in_string;
  loc->word_end_pos = word_end_in_string;


  return STRING_PARSE_SUCCESS;
}


/*char * findWord (char * string, char * word)*/
/*{*/
/*  enum whichRegex whichregex;*/
/*  char * tempWord = word;*/
/*  for (; *tempWord; tempWord++)*/
/*  {*/
/*    whichregex = regexpParse(tempWord);*/
/*    if (whichregex != NONE && whichregex != ERROR)*/
/*    {*/
/*      break;*/
/*    }*/
/*  }*/
/**/
/*  if (whichregex == NONE || whichregex == ERROR)*/
/*  {*/
/*    return findWord(string, word);*/
/*  }*/
/*  else */
/*  {*/
/*    return findWordInStringRegex(string, word);*/
/*  }*/
/*}*/
