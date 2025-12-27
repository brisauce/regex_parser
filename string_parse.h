#ifndef STRING_PARSE_H
#define STRING_PARSE_H

#define STRING_PARSE_FAIL -1
#define STRING_PARSE_SUCCESS 1 

char * findWordInString(char * string, char * word);

int findWordInStringRegex(char * string, char * word, char ** start, char ** end);

#endif
