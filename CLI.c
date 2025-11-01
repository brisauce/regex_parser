#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "build/string_processorConfig.h"

void printHelp()
{
  printf("\
%s version %d.%d.%d\n\
\n\
Example usage:\n\
%s \"example string\"\n\
\n\
This program takes as argument one string argument surrounded by double quotes. WIP.",
         PROJECT_NAME,
         string_processor_VERSION_MAJOR,
         string_processor_VERSION_MINOR,
         string_processor_VERSION_PATCH,
         PROJECT_NAME);
}

void parseCLI(int argc, char ** argv)
{
  for (int i = 1; i < argc; i++)
  {
    if (!strcmp("--help", argv[i]) || !strcmp("--h", argv[i]) || !strcmp("-h", argv[i]))
    {
      printHelp();
      exit(EXIT_SUCCESS);
    }
  }
}
