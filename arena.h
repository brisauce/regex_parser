#ifndef ARENA_H
#define ARENA_H
#include <stdio.h>
#include <stdbool.h>

typedef struct {
  FILE * fp;
  //  regexp containing word
  char * word;
  //  word replacing word parsed for
  char * new_word;
  //  flag to run regex test
  bool regex_test;
} arena;

arena * arenaInit(void);
void arenaDestroy(arena * s);

#endif 
