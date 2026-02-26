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
  MATCH_ONE_OR_MORE_CHARS_WITH_ESCAPE_CHAR,
  MATCH_ZERO_OR_ONE_CHAR_WITH_ESCAPE_CHAR, 
  NONE,
};

bool isInCharClass(char c, char A, char Z);

char * parseCharPattern(char * string, char * A, char * Z) __attribute_warn_unused_result__;

enum whichRegex regexpParse(char * string);

#endif
