#ifndef ARENA_H
#define ARENA_H
#include <stdio.h>
#include <stdbool.h>
#include "word_loc_struct.h"

typedef struct {
  FILE * fp;

  //  regexp containing word
  char * word;

  //  word replacing word parsed for
  char * new_word;

  //  Name of file used for query-replace
  char * file_name;

  //  flag to run regex test
  bool regex_test;

  //  flag to log file position indicators used 
  //  to indicate where words are
  bool log_pointers;

  //  Flag to suppress terminal text output
  bool quiet;

  //  dynamic array used to store data for pointers to beginning and end of words
  word_loc * locDynArray;
} arena;

arena * arenaInit(void);
void arenaDestroy(arena * s);

#endif 
