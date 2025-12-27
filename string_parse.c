#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "string_parse.h"
#include "regexp.h"
#include "error_handling.h"

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

int findWordInStringRegex(char * string, char * word, char ** start, char ** end)
{
  /*
   *  Parses a string for a char sequence which matches a regex containing word.
   *
   *  Returns -1 (STRING_PARSE_FAIL) if this fails due to a passed regex word which has poorly 
   *  implemented regex.
   *
   *  Otherwise, returns a positive number.
   */

  if (*string == '*')
  {
    setErrorState(MATCH_ANY_NUM_MISSING_ARG);
    return STRING_PARSE_FAIL;
  }

  if (*string == '?')
  {
    setErrorState(MATCH_ONE_CHAR_MISSING_ARG);
    return STRING_PARSE_FAIL;
  }

  //  Optimization- allows the regex parse function to only run when the word pointer changes
  char * current_word_index = NULL;

  //  The state of which regex is currently being looked at in the word
  enum whichRegex state;

  //  Remembers where the start of the word is so it can be returned to upon a mismatch
  const char * word_start = word;

  //  Tracks the location of the identified word and returns its location to the user 
  char * word_start_in_string = NULL;
  char * word_end_in_string = NULL;

  for (; *string && *word; string++)
  {

    //  Step through the string, searching for the word. 
    //  Once the first character in the word is detected, start moving through the word, checking 
    //  character by character. The word pointer will only be advanced if there is a match.
    //
    //  If any letter past the first letter does not match, stop parsing the word and move the pointer 
    //  for the word back to the start of the word. Start searching for the first letter in the word again. 
    //
    //  if the regex word is formatted incorrectly, return.
    //
    //  Do this until either the end of the string or the word is reached.

    if (current_word_index != word)
    {
      //  This prevents the program from needing to check the word for every iteration of the loop
      state = regexpParse(word);
      current_word_index = word;
    }

    switch (state) 
    {
    case NONE:
      if (*string == *word)
      {
        if (word == word_start)
        {
          word_start_in_string = string;
        }
        word++;
      }
      else if (word == word_start)
      {
        break;
      }
      else 
      {
        word = (char *) word_start;
      }
      break;
    case CHAR_PATTERN:
    {
      char A;
      char Z;
      word = parseCharPattern(word, &A, &Z);
      if (!A || !Z)
      {
        //  Error in parsing or invalid char class
        return STRING_PARSE_FAIL;
      }

      if (!isInCharClass(string, A, Z))
      {
        //  String doesn't match word, start looking for the word again from the start
        word = (char *) word_start;
      }
      else 
      {
        if (word == word_start)
        {
          word_start_in_string = string;
        }

        //  parseCharPattern will have advanced the word pointer up to the ']' part in the char 
        //  pattern regexp, this will move it past that so we are completely past it
        word ++;
      }
      break;
    }
    case MATCH_ONE_OR_MORE_CHARS: 
    {
      //  The first two chars in the word will be the argument, i.e. what character we are looking for,
      //  followed by the regex character.

      char * start = string;
      for(; *string && *string == *word; string++){}

      if (string == start)
      {
        word = (char *) word_start;
      }
      else 
      {
        if (word == word_start)
        {
          word_start_in_string = string;
        }


        word += 2;
      }

      break;
    }
    case MATCH_ZERO_OR_ONE_CHAR:
      //  The first two chars in the word will be the argument followed by the regex character

      if (*word != *string)
      {
        //  if the character we are looking for is not present, this still satisfies the regex,
        //  but since the main loop will advance the string pointer and it may contain data we 
        //  need to match, we decrement it so the string pointer ends up where it belongs
        string --;
      }
      else if (word == word_start)
      {
        word_start_in_string = string;
      }

      word += 2;
      break;
    case MATCH_ONE_OR_MORE_CHARS_WITH_ESCAPE_CHAR:
    {
      //  The first three characters of the word are all part of a regex- the first is 
      //  the slash indicating an escape character, the second is the character we are looking 
      //  for, and the third is the regex character.

      char * start = string;
      for(; *string && *string == word[1]; string++){}

      if (string == start)
      {
        word = (char *) word_start;
      }
      else 
      {
        if (word == word_start)
        {
          word_start_in_string = string;
        }

        word += 3;
      }

      break;
    }
    case MATCH_ZERO_OR_ONE_CHAR_WITH_ESCAPE_CHAR:

      if (*string != word[1])
      {
        string --;
      }
      else if (word == word_start)
      {
        word_start_in_string = string;
      }

      word += 3;

      break;
    case ESCAPE_CHAR:

      if (*string != word[1])
      {
        word = (char *) word_start;
      }
      else if (word == word_start)
      {
        word_start_in_string = string;
      }
      else 
      {
        word += 3;
      }

      break;
    case ERROR:
      return STRING_PARSE_FAIL;
    }
  }

  if (word_start + strlen(word_start) == word)
  {
    word_end_in_string = string;
  }

  *start = word_start_in_string;
  *end = word_end_in_string;


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
