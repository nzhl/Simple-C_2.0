#ifndef ERROR_H
#define ERROR_H

#include "ncc.h"


void error_print(char *line, char *pos, char *fmt, ...);
void warning_print(char *line, char *pos, char *fmt, ...);
char *token_pos_tostring(Token *tok);

#define STR2(x) #x
#define STR(x) STR2(x)

#define error(...) error_print(__FILE__ ":" STR(__LINE__), NULL, __VA_ARGS__)
#define errort(tok, ...) error_print(__FILE__ ":" STR(__LINE__), token_pos_tostring(tok), __VA_ARGS__)
#define warning(...) warning_print(__FILE__ ":" STR(__LINE__), NULL, __VA_ARGS__)
#define warningt(tok, ...) warning_print(__FILE__ ":" STR(__LINE__), token_pos_tostring(tok), __VA_ARGS__)


#endif
