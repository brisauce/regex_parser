#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

/*
 *  IMPORTANT NOTE PLEASE READ
 *
 *
 *
 *  Whenever a new error state is added to this enum, a corresponding error message MUST 
 *  be added to the array of strings in error_handling.c or the error messaging system 
 *  WILL NOT WORK.
 */

enum error_state {
  NO_ERROR_STATE = 0,
  INVALID_ESCAPE_CHAR,
  EXPECTED_CLOSING_BRACE,
  WORD_NOT_FOUND,
  INVALID_CHAR_CLASS,
  MATCH_ANY_NUM_MISSING_ARG,
  MATCH_ONE_CHAR_MISSING_ARG,

  // dynamic array
  DYNAMIC_ARRAY_ADD_FAIL,

  //  system errors
  FAILED_MEMORY_ALLOC,
  FAILED_FILE_OPEN,
  FAILED_FILE_WRITE
  //  whatever the last enum in this list is should be used as the array size for 
  //  the error messages in error_handling.c
};

void setErrorState(enum error_state state);
enum error_state getCurrentErrorState(void);
void printErrorState(char * errmsg);

#endif
