#include <stdlib.h>

#include "arena.h"
#include "dynamic_array.h"

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

  if (s->locDynArray)
  {
    dynArrayDestroy((void **) &s->locDynArray);
  }

  free(s);
}
