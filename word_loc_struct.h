#ifndef WORD_LOC_STRUCT_H
#define WORD_LOC_STRUCT_H

typedef struct {
  //  Stores the file position indicator returned by ftell() in <stdio.h>
  long word_start_pos;
  long word_end_pos;
} word_loc;

#endif
