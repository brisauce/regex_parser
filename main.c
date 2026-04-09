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
 *  Do query-replace with regexp against a specific word? DONE
 *
 *  Extend a regexp with grouping?
 *
 *  Extend query-replace with grouping?
 *
 */

#include <stdio.h>
#include <string.h>

#include "arena.h"
#include "build/string_processorConfig.h"
#include "CLI.h"
#include "string_parse.h"
#include "replace_word.h"
#include "error_handling.h"
#include "regexp.h"
#include "dynamic_array.h"
#include "word_loc_struct.h"
#include "file_io.h"

#define ARRAY_ELEMENTS 10

enum logPointersOptions{
  NO_INCOMING_CHANGE = 0,
  BEFORE_POINTERS_ADJUSTED,
  AFTER_POINTERS_ADJUSTED
};

void regexDetectTest(char * string)
{
  char * test_regex = string;
  const unsigned int dont_use_me;

  printf("Testing string \"%s\" for regular expressions.\n", string);

  enum whichRegex result = regexpParse(test_regex, (unsigned int *) &dont_use_me);
  
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
    case NONE:
      desc = "NONE";
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

void findWords(arena * a)
{

  word_loc loc;

  while (true)
  {

    if (findWordInStringRegex(a, &loc) == STRING_PARSE_FAIL)
    {
      printErrorState("Error in findWordInStringRegex:");
      return;
    }

    if (loc.word_start_pos == NOT_FOUND || loc.word_end_pos == NOT_FOUND)
    {
      break;
    }

    if (!dynArrayAdd(&loc))
    {
      setErrorState(DYNAMIC_ARRAY_ADD_FAIL);
      printErrorState("Error:");
      return;
    }

    //  Move the file pointer past the end of the word last found
    fseek(a->fp, loc.word_end_pos + sizeof(char), SEEK_SET);
  } 
}

void printSeparator(FILE * fp, size_t num)
{
  #define LINES 6
  char * diamond[LINES] = {
    "  /\\  ",
    " //\\\\ ", 
    "//  \\\\", 
    "\\\\  //",
    " \\\\// ",
    "  \\/  "
  };

  for (size_t i = 0; i < LINES; i++)
  {
    for (size_t j = 0; j < num; j++)
    {
      fputs(diamond[i], fp);
    }
    fprintf(fp, "\n");
  }
}

void logPointers(arena * a, char * funcName, enum logPointersOptions option)
{
  FILE * fp = fopen("file_pos_pointers.txt", "a");
  static bool pre_change_pointers_logged = false;
  static bool first_time_running = true;

  if (!fp)
  {
    printf("[%-20s] ERROR: Failed to open file_pos_pointers.txt to log pointers. Exiting", __func__);
    exit(EXIT_FAILURE);
  }

  if (first_time_running)
  {
    printSeparator(fp, 6); 
    first_time_running = false;
  }

  fprintf(fp, "--Logging pointers for %s\n", funcName);

  if (option == BEFORE_POINTERS_ADJUSTED || option == AFTER_POINTERS_ADJUSTED)
  {
    fprintf(fp, "--This is %s pointers to words are adjusted in the file.\n", 
            (option == BEFORE_POINTERS_ADJUSTED) ? "BEFORE" : "AFTER");
  }

  fprintf(fp, "--Regexp word searched against file:\n%s\n\n", a->word);

  if (a->new_word)
  {
    fprintf(fp, "--New word to replace found words:\n%s\n\n", a->new_word);
  }
  
  unsigned int limit = dynArrayGetArraySize();

  for (unsigned int i = 0; i < limit; i++)
  {
    word_loc loc = *(word_loc *) dynArrayGetData(i);
    fprintf(fp, "Ptr set %2u: Start %4ld End %4ld\n", 
            i + 1, 
            loc.word_start_pos, 
            loc.word_end_pos);
  }

  if (option == BEFORE_POINTERS_ADJUSTED)
  {
    pre_change_pointers_logged = true;
  }

  fclose(fp);
}

void printFoundWords (arena * a)
{
  word_loc * read;

  while ( (read = (word_loc *) dynArrayRead(NULL)) )
  {
    fseek(a->fp, read->word_start_pos, SEEK_SET);

    //  If the position pointer is one beyond the index where the word ends, the word 
    //  has been read out.
    putchar('\"');
    while (ftell(a->fp) != read->word_end_pos + sizeof(char))
    {
      char c = fgetc(a->fp);

      if (c == EOF)
      {
        fputs("EOF", stdout);
      }
      else
      {
        putchar(c);
      }
    }
    puts("\"\n");
  }
}

void replaceWords (arena * a)
{
  word_loc * read;

  long word_size_diff;
  long old_word_size;
  long new_word_size = strlen(a->new_word);

  if (dynArrayGetArraySize() > 0)
  {

    unsigned int array_size = dynArrayGetArraySize();
    for (unsigned int i = 1; i < array_size; i++) 
    {
      read = dynArrayGetData(i);

      old_word_size = read->word_end_pos - read->word_start_pos;

      if (old_word_size == new_word_size)
      {
        continue;
      }

      word_size_diff = old_word_size - new_word_size;

      //  Instead of looping through every time you change the position of a word, you 
      //  can instead change all the positions of the words beforehand by determining the
      //  number of words that will be changed and changing the positions of the words 
      //  based on their position in the array.
      read->word_start_pos = (read->word_start_pos - (word_size_diff * i)) - i;
      read->word_end_pos = (read->word_end_pos - (word_size_diff * i)) - i;
    }
  }

  while ( (read = (word_loc *) dynArrayRead(NULL)) )
  {
    replaceWordinFile(a, *read);
  }

}

int main (int argc, char ** argv)
{

  arena * a = parseCLI(argc, argv);

  if (a->regex_test)
  {
    regexDetectTest(a->word);
    arenaDestroy(a);
    exit(EXIT_SUCCESS);
  }

  //  Dynamic array to hold pointers to the first and last character pointers of every word 

  if (!dynArrayInit(ARRAY_ELEMENTS, sizeof(word_loc)))
  {
    printf("Failed to initialize dynamic array! Exiting\n");
    arenaDestroy(a);
    return EXIT_FAILURE;
  }

  findWords(a);

  if (getCurrentErrorState() != NO_ERROR_STATE)
  {
    return EXIT_FAILURE;
  }


  if (!a->new_word)
  {
    //  In the case where the user just wants to find the location of the words queried
    if (a->log_pointers)
    {
      logPointers(a, "printFoundWords", NO_INCOMING_CHANGE);
    }

    printFoundWords(a);
  }
  else 
  {
    //  query-replace
    if (a->log_pointers)
    {
      logPointers(a, "replaceWords", BEFORE_POINTERS_ADJUSTED);
    }

    replaceWords(a);

    if (a->log_pointers)
    {
      logPointers(a, "replaceWords", AFTER_POINTERS_ADJUSTED);
    }
  }

  dynArrayDestroy();
  arenaDestroy(a);
}
