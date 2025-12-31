/*  Text processing in strings 
 *
 *  Can you search for a given word in a string? 
 *  DONE
 *
 *  Can you replace a word in a string and return a copy with new contents? 
 *  DONE
 *
 *  Can you implement some regular-expression-matching functions for strings? For example, 
 *  find a character class such as [A-Q], match with * (meaning "one or more occurances of 
 *  the preceding character") or match with ? (meaning "zero or one occurance of the 
 *  preceding character")
 *
 *  OR
 *
 *  Can you implement a regular-expression-matching function for POSIX character classes such 
 *  as [[:alpha:]], [[:digit:]] and so on? 
 *
 *  Can you stitch all of these functionalities together to search for a regular expression 
 *  in a string? 
 *  DONE
 *
 *  Do query-replace with regexp against a specific word?
 *
 *  Extend a regexp with grouping?
 *
 *  Extend query-replace with grouping?
 *
 */

#include <stdio.h>
#include <string.h>

#include "build/string_processorConfig.h"
#include "CLI.h"
#include "string_parse.h"
#include "replace_word.h"
#include "error_handling.h"
#include "regexp.h"
#include "dynamic_array.h"

typedef struct {
  char * start;
  char * end;
} word_loc;


void printNChars(char * string, unsigned int n)
{
  for (unsigned int i = 0; i < n && string[i]; i++)
  {
    putc(string[i], stdout);
  }
  putc('\n', stdout);
  fflush(stdout);
}

void regexDetectTest(char * string)
{
  char * test_regex = string;

  enum whichRegex result = regexpParse(test_regex);
  
  char * desc;
  switch (result)
  {
    case ERROR:
      desc = "ERROR";
      break;
    case CHAR_PATTERN:
      desc = "CHAR_PATTERN";
      break;
    case MATCH_ONE_OR_MORE_CHARS:
      desc = "MATCH_ONE_OR_MORE_CHARS";
      break;
    case MATCH_ZERO_OR_ONE_CHAR:
      desc = "MATCH_ZERO_OR_ONE_CHAR";
      break;
    case ESCAPE_CHAR:
      desc = "ESCAPE_CHAR";
      break;
    case MATCH_ONE_OR_MORE_CHARS_WITH_ESCAPE_CHAR:
      desc = "MATCH_ONE_OR_MORE_CHARS_WITH_ESCAPE_CHAR";
      break;
    case MATCH_ZERO_OR_ONE_CHAR_WITH_ESCAPE_CHAR:
      desc = "MATCH_ZERO_OR_ONE_CHAR_WITH_ESCAPE_CHAR";
      break;
    case NONE:
      desc = "NONE";
      break;
  }

  printf("Test regex: %s\n", test_regex);
  puts(desc);

  if (result == CHAR_PATTERN)
  {
    char A;
    char Z;
    parseCharPattern(test_regex, &A, &Z);
    printf("Range pulled from passed char pattern: [%c-%c]\n", A, Z);
  }

  if (getCurrentErrorState() != NO_ERROR_STATE)
  {
    printErrorState("Error in parseCharPattern:");
    return;
  }
}

void displayNChars (char * string, unsigned int n)
{
  for (unsigned int i = 0; i < n; i++)
  {
    printf("%c%c", string[i], (i == n - 1) ? '\n' : '\0');
  }
}

#define ARRAY_ELEMENTS 10

int main (int argc, char ** argv)
{
  if (argc != 3)
  {
    printf("Need 2 arguments: first, text to parse; Second, word to use to parse the text.\n");
    printf("If either argument contains more than one word separated by a space, wrap that \
argument in quotations.\n");
    exit(EXIT_FAILURE);
  }

  char * start;
  char * end;

  char * string = argv[1];

  if (!dynArrayInit(ARRAY_ELEMENTS, sizeof(word_loc)))
  {
    printf("Failed to initialize dynamic array! Exiting\n");
    return EXIT_FAILURE;
  }

  do {

    if (findWordInStringRegex(string, argv[2], &start, &end) == STRING_PARSE_FAIL)
    {
      printErrorState("Error in findWordInStringRegex:");
      return EXIT_FAILURE;
    }

    word_loc temp;
    temp.start = start;
    temp.end = end;

    if (start && end)
    {
      if (!dynArrayAdd(&temp))
      {
        printf("Failed to append an item to dynamic array! Exiting\n");
        return EXIT_FAILURE;
      }
    }

    string = end + 1;

  } while (start && end);

  word_loc * read;

  while (read = (word_loc *) dynArrayRead(NULL))
  {
    displayNChars(read->start, read->end - read->start);
  }

  dynArrayDestroy();
}
