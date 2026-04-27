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
 *  Extend a regexp with grouping? DONE
 *
 *  Extend query-replace with grouping?
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

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

void printFoundWords (arena * a, FILE * stream);


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

    dynArrayAdd((void *) &a->locDynArray, &loc);

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
      /*fprintf(fp, diamond[i]);*/
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

  if (first_time_running)
  {
    fputs("Words found:\n", fp);
    printFoundWords(a, fp);
  }
  
  unsigned int limit = dynArrayGetArraySize(a->locDynArray);
  bool passed_first_loc = false;
  word_loc prev_loc;
  for (unsigned int i = 0; i < limit; i++)
  {
    word_loc loc = a->locDynArray[i];
    if (passed_first_loc)
    {
      fprintf(fp, "-> Distance: %ld\n", loc.word_start_pos - prev_loc.word_end_pos);
    }
    fprintf(fp, "Ptr set %2u: Start %4ld End %4ld\n", 
            i + 1, 
            loc.word_start_pos, 
            loc.word_end_pos);

    if (!passed_first_loc)
    {
      passed_first_loc = true;
    }
    prev_loc = loc;
  }

  fputc('\n', fp);

  if (first_time_running)
  {
    first_time_running = false;
  }

  if (option == BEFORE_POINTERS_ADJUSTED)
  {
    pre_change_pointers_logged = true;
  }

  fclose(fp);
}

void printFoundWords (arena * a, FILE * stream)
{
  for (size_t i = 0; i < dynArrayGetArraySize(a->locDynArray); i++ )
  {
    word_loc * read = &a->locDynArray[i];
    fseek(a->fp, read->word_start_pos, SEEK_SET);

    //  If the position pointer is one beyond the index where the word ends, the word 
    //  has been read out.
    fprintf(stream, "#%zu", i + 1);
    fputs(" \"" , stream);
    while (ftell(a->fp) != read->word_end_pos + sizeof(char) && !feof(a->fp))
    {
      char c = fgetc(a->fp);

      if (c == EOF)
      {
        fputs("[EOF]", stream);
      }
      else
      {
        fputc(c, stream);
      }
    }
    fputs("\"\n\n", stream);
  }
}

void checkPointerSpacing(arena * a)
{
  size_t array_size = dynArrayGetArraySize(a->locDynArray);
  FILE * fp = fopen("file_pos_pointers_spacing.txt", "w");
  assert(fp);


}

void replaceWords (arena * a)
{
  unsigned int array_size = dynArrayGetArraySize(a->locDynArray);
  char * temp_file_name = "temp"; 

  if (!array_size)
  {
    return;
  }

  //  Every word has a start and end pointer 
  //
  //  Every word has to be pushed forward by the difference in the lengths of each previous 
  //  word and the word that is replacing it
  //
  //  we could do this by:
  //  - starting from the first word to be replaced, overwrite the word from its start 
  //    pointer. determine the difference between the end pointer and where the new 
  //    end of the word is, and adjust every following start and end value by that amount

  for (size_t i = 0; i < array_size; i++)
  {
    //  replace word
    //
    //  determine distance between new pointer and old pointer

    fseek(a->fp, a->locDynArray[i].word_end_pos + sizeof(char), SEEK_SET);

    FILE * tempfp = fopen(temp_file_name, "w+"); 

    if (copyFile(a->fp, ftell(a->fp), tempfp, 0) == COPYFILE_ERROR)
    {
      printf("[%-20s] ERROR within copyfile. Exiting\n", __func__);
      arenaDestroy(a);
      exit(EXIT_FAILURE);
    }

    fseek(a->fp, a->locDynArray[i].word_start_pos, SEEK_SET);

    fputs(a->new_word, a->fp);

    long word_size_diff = a->locDynArray[i].word_end_pos - (ftell(a->fp) - sizeof(char));

    if (word_size_diff)
    {
      for (size_t j = i + 1; j < array_size; j++)
      {
        //  adjust all following pointer based on difference between old and 
        //  new end pointers 
        a->locDynArray[j].word_start_pos -= word_size_diff;
        a->locDynArray[j].word_end_pos -= word_size_diff;
      }
    }
    //  copy the temp file contents back into place in the old file

    copyFile(tempfp, 0, a->fp, ftell(a->fp));
    fclose(tempfp);
    fflush(a->fp);
  }

  remove(temp_file_name);
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

  a->locDynArray = dynArrayInit(ARRAY_ELEMENTS, sizeof(word_loc)); 

  if (!a->locDynArray)
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

    printFoundWords(a, stdout);
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

  dynArrayDestroy((void **)&a->locDynArray);
  arenaDestroy(a);
}
