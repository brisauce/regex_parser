#ifndef FILE_IO_H
#define FILE_IO_H
#include <stdio.h>
void displayCurFileChar(FILE * fp);
void displayNFileChars(FILE * fp, size_t N);
void displayPlaceInFile(FILE * fp);
long getFileEnd(FILE * fp);
void displayPrevFileChar(FILE * fp);

#endif
