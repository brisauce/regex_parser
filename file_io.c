#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

void displayCurFileChar(FILE * fp)
{
  long return_here = ftell(fp);

  char c = fgetc(fp);

  if (c == EOF)
  {
    puts("EOF");
  }
  else
  {
    printf("%c\n", c);
  }

  fseek(fp, return_here, SEEK_SET);
}

void displayNFileChars(FILE * fp, size_t N)
{
  long return_here = ftell(fp);

  for (size_t i = 1; i <= N; i++)
  {
    char c = fgetc(fp);

    if (c == EOF)
    {
      printf("#%3lu EOF\n", i);
    }
    else
    {
      printf("#%3lu %3c\n", i, c);
    }
  }

  fseek(fp, return_here, SEEK_SET);
}

void displayPrevFileChar(FILE * fp)
{
  long return_here = ftell(fp);

  if (return_here == 0)
  {
    //  Start of file, can't go backwards from 0
    printf("[%-20s] Unable to collect previous char in file, \
position indicator is already at the start\n", __func__);
    return;
  }

  fseek(fp, -1, SEEK_CUR);
  
  char c = fgetc(fp);

  if (c == EOF)
  {
    puts("EOF");
  }
  else
  {
    printf("%c\n", c);
  }

  fseek(fp, return_here, SEEK_SET);
}

void displayPlaceInFile(FILE * fp)
{
  long return_here = ftell(fp);

  rewind(fp);

  size_t column = 0;

  #define NOT_FOUND SIZE_MAX

  size_t arrow_index = NOT_FOUND;
  while (!feof(fp))
  {
    bool is_here = ftell(fp) == return_here;
    
    char c = fgetc(fp);

    if (is_here)
    {
      arrow_index = column;
    }

    if (c == EOF)
    {
      break;
    }
    else
    {
      putchar(c);
    }

    if (c == '\n')
    {
      if (arrow_index == NOT_FOUND)
      {
        goto not_found; 
      }

      for (column = 0;; column ++)
      {
        if (column != arrow_index)
        {
          putchar('-');
        }
        else
        {
          putchar('^');
          break;
        }
      }

      arrow_index = NOT_FOUND;
      
    not_found:

      putchar('\n');

      column = 0;
    }
    else 
    {
      column ++;
    }
  }

  fseek(fp, return_here, SEEK_SET);
}

long getFileEnd(FILE * fp)
{
  long return_here = ftell(fp);

  fseek(fp, 0, SEEK_END);

  long retval = ftell(fp);

  fseek(fp, return_here, SEEK_SET);

  return retval;
}
