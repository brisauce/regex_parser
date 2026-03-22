#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "build/string_processorConfig.h"
#include "word_loc_struct.h"
#include "arena.h"
#include "error_handling.h"

void printProjNameAndVersion (void)
{
  printf("%s version %d.%d.%d\n",
         PROJECT_NAME,
         string_processor_VERSION_MAJOR,
         string_processor_VERSION_MINOR,
         string_processor_VERSION_PATCH);
}

void printHelp(void)
{
  printProjNameAndVersion();
  printf("\
Example usage:\n\
%s (options) \"word or phrase to search for\" \"example_file_name.whatever\" \"word or phrase to replace searched word\"\n\
\n\
This program takes as argument two string arguments surrounded by double quotes.\n\
\tThe first argument after any options must be the word or phrase to search for.\n\
This word or phrase can be any length and may include regular expressions.\n\
If this would include spaces, the argument must be surrounded by double quotes.\n\
The regular expressions supported by this program can be viewed by passing the --regexp_supported argument.\n\
\n\
\tThe second argument after options will be the name of the text file to parse. \n\
The text must be in ASCII one-byte-per-character format or undefined behavior will occur.\n\
\n\
\tThe final argument to pass is the word or phrase to replace the queried word with.\n\
This word will replace every instance of the word in the query argument in the text file,\n\
And should be in double quotes if there are spaces involved.\n",
         PROJECT_NAME);
}

void printRegexSupport(void)
{
  printf("\
Supported regular expressions by this program in the \"word or phrase to search for\" argument are:\n\
\t\\\\, \\*, \\?, \\[, \\-, \\] : escape character. There are some characters\n\
which, unless pointed out to the program, will be treated as if they are part of a regular\n\
expression instead of parsed as a standalone character. When a character listed previously\n\
following the back slash is placed in the \"word or phrase to search for\" argument\n\
FOLLOWING a back slash character, it will be treated as a standalone character to be searched\n\
for instead of part of a regular expression.\n\
\n\
\t? : will search for zero or one instances of the character prefixed to the \"?\".\n\
This also supports an escape character as the prefix.\n\
\n\
\t* : will search for one or more instances of the character prefixed to the \"*\".\n\
This also supports an escape character as the prefix.\n\
\n\
\t[a-z] : will search for a single instance of a character in the range a-z, with the\n\
range defined by the character's numerical value on the ascii table.\n\
For a printout of the ascii table's printable characters in the terminal, pass\n\
the option of --ascii-table.\n\
");
}

void printAsciiTable(void)
{
  //  prints all printable characters in the ascii table.
  puts("\
While this program does not support the input of characters in decimal format, the character's\n\
decimal equivalent is printed nonetheless to display these characters' relationship to one another.\n");
  puts("dec|char|notes");
  for (char c = ' '; c <= '~'; c++)
  {
    if (c == ' ')
    {
      printf("%3d ' '  (space)\n", (int) c);
    }
    else
    {
      printf("%3d %3c\n", (int) c, c);
    }
  }
}

arena * parseCLI(int argc, char ** argv)
{
  //  Parses the CLI. Can exit on its own. is the only function allowed to do this.
  if (argc == 1)
  {
    printProjNameAndVersion();
    puts("Pass --help as an option for help in using this program.");
  }

  if (argc > 4)
  {
    assert("WIP need to handle argc > 3!" && NULL);
  }

  arena * a = arenaInit();

  if (!a)
  {
    setErrorState(FAILED_MEMORY_ALLOC);
    printErrorState((char *) __func__);
    exit(EXIT_FAILURE);
  }

  FILE * temp_fp;
  for (int i = 1; i < argc; i++)
  {
    if (!strcmp("--help", argv[i]) || !strcmp("--h", argv[i]) || !strcmp("-h", argv[i]))
    {
      printHelp();
      arenaDestroy(a);
      exit(EXIT_SUCCESS);
    }
    else if (!strcmp("--regex_supported", argv[i]))
    {
      printRegexSupport();
      arenaDestroy(a);
      exit(EXIT_SUCCESS);
    }
    else if (!strcmp("--ascii-table", argv[i]))
    {
      printAsciiTable();
      arenaDestroy(a);
      exit(EXIT_SUCCESS);
    }
    else if (!strcmp("--regex-test", argv[i]))
    {
      if (!a->word)
      {
        printf("To use the regex test feature, pass the word to test before --regex_test.\nExiting\n");
        arenaDestroy(a);
        exit(EXIT_FAILURE);
      }
      a->regex_test = true;
      goto Return;
    }
    else if (!a->word) 
    {
      a->word = argv[i];
    }
    else if ( ( temp_fp = fopen(argv[i], "r+")) && !a->fp)
    {
      a->fp = temp_fp;
    }
    else if (!a->new_word)
    {
      a->new_word = argv[i];
    }
    else
    {
      assert("Unreachable! CLI.c" && NULL);
    }
  }

  if (!a->word)
  {
    puts("No word to search for was passed to the program. Exiting.");
    arenaDestroy(a);
    exit(EXIT_FAILURE);
  }

  if (!a->fp)
  {
    puts("No file was identified to parse. Exiting.");
    arenaDestroy(a);
    exit(EXIT_FAILURE);
  }

  Return:
  return a;
}


