#ifndef STRING_PARSE_H
#define STRING_PARSE_H
#include <limits.h>

#include "arena.h"
#include "word_loc_struct.h"

//  Indicator for whether findWordInStringRegex() has exited successfully
#define STRING_PARSE_FAIL -1
#define STRING_PARSE_SUCCESS 1 

//  Indicator for findWordInStringRegex if it doesn't find the beginning or end of a word
#define NOT_FOUND LONG_MAX
char * findWordInString(char * string, char * word);

int findWordInStringRegex(arena * a, word_loc * loc);

#endif
