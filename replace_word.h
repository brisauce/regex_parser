#ifndef REPLACE_WORD_H
#define REPLACE_WORD_H

#include "arena.h"
#include "word_loc_struct.h"

#define REPLACE_WORD_ERROR -1
#define REPLACE_WORD_SUCCESS 1

int replaceWordinFile(arena * a, word_loc loc);
int copyFile(FILE * src, long src_ptr, FILE * dest, long dest_ptr);

#endif
