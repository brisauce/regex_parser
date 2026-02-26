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

#include "arena.h"
#include "build/string_processorConfig.h"
#include "CLI.h"
#include "string_parse.h"
#include "replace_word.h"
#include "error_handling.h"
#include "regexp.h"
#include "dynamic_array.h"
#include "word_loc_struct.h"

#define ARRAY_ELEMENTS 10

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

void copyFileTest (arena * a)
{
  // copy the entire file to the end of the original file

  char * temp_file_name = "temp";
  FILE * temp_file = fopen(temp_file_name, "r+");
}

int main (int argc, char ** argv)
{

  arena * a = parseCLI(argc, argv);
  /**/
  /*copyFileTest(a);*/
  /**/
  /*arenaDestroy(a);*/
  /**/
  /*return */
  /**/
  //  Dynamic array to hold pointers to the first and last character pointers of every word 

  if (!dynArrayInit(ARRAY_ELEMENTS, sizeof(word_loc)))
  {
    printf("Failed to initialize dynamic array! Exiting\n");
    arenaDestroy(a);
    return EXIT_FAILURE;
  }

  word_loc loc;

  while (true)
  {

    if (findWordInStringRegex(a, &loc) == STRING_PARSE_FAIL)
    {
      printErrorState("Error in findWordInStringRegex:");
      return EXIT_FAILURE;
    }

    if (loc.word_start_pos != NOT_FOUND && loc.word_end_pos != NOT_FOUND)
    {
      if (!dynArrayAdd(&loc))
      {
        printf("Failed to append an item to dynamic array! Exiting\n");
        dynArrayDestroy();
        arenaDestroy(a);
        return EXIT_FAILURE;
      }
    }
    else 
    {
      break;
    }

    //  Move the file pointer past the end of the word last found
    fseek(a->fp, loc.word_end_pos + sizeof(char), SEEK_SET);

  } 

  word_loc * read;

  if (!a->new_word)
  {
    //  In the case where the user just wants to find the location of the words queried

    while ( (read = (word_loc *) dynArrayRead(NULL)) )
    {
      fseek(a->fp, read->word_start_pos, SEEK_SET);

      //  If the position pointer is one beyond the index where the word ends, the word 
      //  has been read out.
      while (ftell(a->fp) != read->word_end_pos + sizeof(char))
      {
        putchar(fgetc(a->fp));
      }

      putchar('\n');
    }
  }
  else 
  {
    //  query-replace

    long word_size_diff;
    long old_word_size = strlen(a->word);
    long new_word_size = strlen(a->new_word);

    if (old_word_size < new_word_size)
    {
      word_size_diff = new_word_size - old_word_size;
    }
    else
    {
      word_size_diff = old_word_size - new_word_size;
    }

    while ( (read = (word_loc *) dynArrayRead(NULL)) )
    {
      replaceWordinFile(a, *read);

      if (!word_size_diff)
      {
        continue;
      }

      unsigned int read_pos = dynArrayGetIndex();
      unsigned int array_size = dynArrayGetArraySize();


      for (unsigned int i = read_pos; i < array_size; i++)
      {
        word_loc * indexed_loc = dynArrayGetData(i);

        if (!indexed_loc)
        {
          printf("Failed to return data from dynamic array index %d! Exiting\n", i);
          arenaDestroy(a);
          dynArrayDestroy();
          return EXIT_FAILURE;
        }

        indexed_loc->word_end_pos += word_size_diff;
        indexed_loc->word_start_pos += word_size_diff;

      }

    }
  }

  dynArrayDestroy();
  arenaDestroy(a);
}
