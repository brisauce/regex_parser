#ifndef REGEXP_H
#define REGEXP_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
enum whichRegex {
  ERROR = 0,
  CHAR_PATTERN,
  MATCH_ONE_OR_MORE_CHARS,
  MATCH_ZERO_OR_ONE_CHAR,
  ESCAPE_CHAR,
  NONE
};

bool isInCharClass(char c, char A, char Z);

char * parseCharPattern(char * word, char * A, char * Z);

bool validEscChar (char * word, unsigned int * len);

bool validCharClass (char * word, unsigned int * len);

enum whichRegex regexpParse(char * word, unsigned int * len);

#endif
