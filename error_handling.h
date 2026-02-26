#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

enum error_state {
  NO_ERROR_STATE = 0,
  INVALID_ESCAPE_CHAR,
  EXPECTED_CLOSING_BRACE,
  WORD_NOT_FOUND,
  INVALID_CHAR_CLASS,
  MATCH_ANY_NUM_MISSING_ARG,
  MATCH_ONE_CHAR_MISSING_ARG,

  //  system errors
  FAILED_MEMORY_ALLOC,
  FAILED_FILE_OPEN,
  FAILED_FILE_WRITE
};

void setErrorState(enum error_state state);
enum error_state getCurrentErrorState(void);
void printErrorState(char * errmsg);

#endif
