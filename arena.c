#include <stdlib.h>

#include "arena.h"

arena * arenaInit(void)
{
  arena * retval = calloc(1, sizeof(arena));

  return retval;
}

void arenaDestroy(arena * s)
{
  if (s->fp)
  {
    fclose(s->fp);
  }

  free(s);
}
