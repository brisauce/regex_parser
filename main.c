/*  Text processing in strings 
 *
 *  Can you search for a given word in a string? 
 *  DONE
 *
 *  Can you replace a word in a string and return a copy with new contents? 
 *  DONE
 *
 *  Can you implement some regular-expression-matching functions for strings? For example, 
 *  find a character class such as [A-Q], match with * (meaning "anything") or match with 
 *  ? (meaning "any character")
 *  OR
 *  Can you implement a regular-expression-matching function for POSIX character classes such 
 *  as [[:alpha:]], [[:digit:]] and so on? 
 *
 *  Can you stitch all of these functionalities together to search for a regular expression 
 *  in a string?
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

void printNChars(char * string, unsigned int n)
{
  for (unsigned int i = 0; i < n && string[i]; i++)
  {
    putc(string[i], stdout);
  }
  putc('\n', stdout);
  fflush(stdout);
}

int main (int argc, char ** argv)
{
  //  regexp API sketch
  //
  //

}
