#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "error_handling.h"
#include "string_parse.h"
#include "regexp.h"

bool isInCharClass(char * string, char A, char Z)
{
  //  determine if the character pointed to is within a certain pattern, and return true if it is

  return *string >= A && *string <= Z;
}

bool validEscChar (char * string)
{
  switch (string[1])
  {
    case '\\': 
    case '*':
    case '?':
    case '[':
      return true;
    default:
      setErrorState(INVALID_ESCAPE_CHAR);
      return false;
  }
}

enum whichRegex regexpParse(char * string)
{
  if (*string == '[')
  {
    //  Character class
    for(;*string != ']' && *string != '\n' && *string != '\0'; string++){}

    if (*string != ']')
    {
      setErrorState(EXPECTED_CLOSING_BRACE);
      return ERROR;
    }
    else 
    {
      return CHAR_PATTERN;
    }
  }
  if (*string == '\\')
  {
    //  Escape character, either as part of a "one or more chars" or "zero or one" regexp 
    //  or by itself

    if (!validEscChar(string))
    {
      return ERROR;
    }

    if (strlen(string) >= 3ul)
    {
      if (string[2] == '*')
      {
        return MATCH_ONE_OR_MORE_CHARS_WITH_ESCAPE_CHAR;
      }
      else if (string[2] == '?')
      {
        return MATCH_ZERO_OR_ONE_CHAR_WITH_ESCAPE_CHAR;
      }
    }
    else
    {
      return ESCAPE_CHAR;
    }
  }
  else if (string[1] == '*')
  {
    return MATCH_ONE_OR_MORE_CHARS;
  }
  else if (string[1] == '?')
  {
    return MATCH_ZERO_OR_ONE_CHAR;
  }

  return NONE;
}

char * advancePntrToLBracket(char * word)
{
  for (;*word != ']' && *word != '\0'; word++)
  {}
  return word;
}

char * parseCharPattern(char * word, char * A, char * Z)
{
  //  ensure a regex for a character pattern is in the form:
  //
  //  [A-Z]
  //
  //  spaces may be included and will not get in the way of parsing the regex.
  //
  //  either *A or *Z being '\0' indicates an error state.
  //
  bool postHyphen = false;
  char a = '\0';
  char z = '\0';
  for (;*word != ']'; word ++)
  {
    if (*word == '[')
    {
      continue;
    }

    if (regexpParse(word) == ESCAPE_CHAR)
    {
      if (!a)
      {
        a = word[1];
      }
      else
      {
        z = word[1];
      }
      word++;
    }
    else if (*word == '-')
    {
      if (a == '\0')
      {
        setErrorState(INVALID_CHAR_CLASS);
        goto Return;
      }
      else
      {
        postHyphen = true;
        continue;
      }
    }
    else if (*word >= '!' && *word <= '~')
    {
      if (z != '\0')
      {
        break;
      }
      else if (postHyphen)
      {
        z = *word;
      }
      else if (a == '\0')
      {
        a = *word;
      }
    }
  }

  if (*word != ']')
  {
    //  the function calling this one will expect the word pointer returned to be pointing 
    //  to the ']' bracket, so if the previous loop didn't make it all the way there we 
    //  have to help it get to the finish line.
    word = advancePntrToLBracket(word);
  }

Return:

  if (a > z && getCurrentErrorState() == NO_ERROR_STATE)
  {
    //  if there is an error state set, the function is saying that there was a problem parsing 
    //  something. Otherwise, there was a bug or some other problem parsing the char class and 
    //  it needs to be flagged.
    setErrorState(INVALID_CHAR_CLASS);
    a = '\0';
  }

  *A = a;
  *Z = z;
  return word;
}

