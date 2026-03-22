#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "error_handling.h"
#include "string_parse.h"
#include "regexp.h"

bool validOneOrMoreChars (char * word, unsigned int * len);
bool validZeroOrOneChar (char * word, unsigned int * len);
bool validCharClass (char * word, unsigned int * len);


bool isInCharClass(char c, char A, char Z)
{
  //  determine if the character pointed to is within a certain pattern, and return true if it is
  return c >= A && c <= Z;
}

bool validEscChar (char * word, unsigned int * len)
{
  if (*word != '\\')
  {
    return false;
  }

  if (strlen(word) < 2)
  {
    //  Word is too short to contain an esc. char.
    return false;
  }

  switch (word[1])
  {
    case '\\': 
    case '*':
    case '?':
    case '[':
    case '-':
    case ']':
      *len = 2;
      return true;
    default:
      return false;
  }
}

//  Implementation note- If a regular expression was not detected or one character has 
//  passed and the regex character in question has been detected, then it's fair to say the 
//  character the regular expresssion is there for is in the previous character spot.

bool validOneOrMoreChars (char * word, unsigned int * len)
{
  //  Check for other regular expressions, then see if they are followed by the 
  //  regular expression for one or more characters.

  if (*word == '*')
  {
    return false;
  }

  unsigned int length;
  char * start = word;
  char * last_sig_char = word;
  for (; *word != '\n' && *word != '\0'; word ++)
  {
    if (word - last_sig_char > 1)
    {
      return false;      
    }
    else if (validCharClass(word, &length))
    {
      word += length - 1;
      last_sig_char += length;
    }
    else if (validEscChar(word, &length))
    {
      word += length - 1;
      last_sig_char += length;
    }
    else if (*word == '*')
    {
      *len = word - start; 
      return true;
    }
  }

  return false;
}

bool validZeroOrOneChar (char * word, unsigned int * len)
{
  //  Check for other regular expressions, then see if they are followed by the 
  //  regular expression for zero or one character.

  if (*word == '?')
  {
    return false;
  }

  unsigned int length;
  char * start = word;
  char * last_sig_char = word;
  for (; *word != '\n' && *word != '\0'; word ++)
  {
    if (word - last_sig_char > 1)
    {
      return false;      
    }
    else if (validCharClass(word, &length))
    {
      word += length - 1;
      last_sig_char += length;
    }
    else if (validEscChar(word, &length))
    {
      word += length - 1;
      last_sig_char += length;
    }
    else if (*word == '?')
    {
      *len = word - start; 
      return true;
    }
  }

  return false;
}

bool validCharClass (char * word, unsigned int * len)
{
  if (*word != '[')
  {
    return false;
  }

  char * start = word;
  unsigned int length;
  bool hyphen = false;

  //  Move the word pointer past the left bracket
  word ++;

  for (; *word != '\n' && *word != '\0'; word++)
  {
    if (validEscChar(word, &length))
    {
      //  a valid escape character may contain ']', so this needs to be skipped
      word += length - 1;
    }
    else if (*word == '-')
    {
      //  Hyphen separating the two arguments to a char class
      hyphen = true; 
    }
    else if (*word == ']' && hyphen)
    {
      *len = word - start;
      return true;
    }
  }

  return false;
}

enum whichRegex regexpParse(char * word, unsigned int * len)
{
  //  This order of checking for regex is intentional.
  //
  //  Since a "?" or "*" regex may be grouped with a escape char. or char. class,
  //  with the escape char. or char. class leading, they need to be validated
  //  for the trailing syntax regex first.
  if (validZeroOrOneChar(word, len))
  {
    return MATCH_ZERO_OR_ONE_CHAR;
  }
  else if (validOneOrMoreChars(word, len))
  {
    return MATCH_ONE_OR_MORE_CHARS;
  }
  else if (validEscChar(word, len))
  {
    return ESCAPE_CHAR;
  }
  else if (validCharClass(word, len))
  {
    return CHAR_PATTERN;
  }
  else 
  {
    return NONE;
  }
}



void parseCharPattern(char * word, char * A, char * Z)
{
  //  Ensure a regex for a character pattern is in the form:
  //
  //  [A-Z]
  //
  //  Either *A or *Z being '\0' indicates an error state.
  //
  //  A valid substitution for 'A' or 'Z' in the above example is an escape character.
  //
  bool postHyphen = false;
  char a = '\0';
  char z = '\0';
  unsigned int dontuseme;
  for (;*word != ']'; word ++)
  {
    if (*word == '[')
    {
      continue;
    }

    if (validEscChar(word, &dontuseme))
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

  assert(*word == ']');

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
}

