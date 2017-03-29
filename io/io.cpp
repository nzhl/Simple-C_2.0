#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "../word/Word.h"


// ------------ Error reporting  part ------------------------

const static char* FilePath = "/Users/zhangzhimin/test.c";

int current_line;

static void handle_exception(const char* fmt, va_list ap){
    char buf[1024];
    vsprintf(buf, fmt, ap);
    printf("\n\n******* Compiler Error ********\n\n"
           "\tFile Name : %s\n"
           "\tLine Number : %d\n"
           "\tReason : %s"
           "\n\n******* Compiler Error ********\n\n"
            , FilePath, current_line, buf);
    exit(0);
}

void error(const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    handle_exception(fmt, ap);
    va_end(ap);
}

// ------------ Lexer helper part  ---------------------------

static FILE *fp = NULL;

int ch = 0;


void get_next_char(){
    if( fp == NULL ){
        fp = fopen(FilePath, "r");
        if(fp == NULL)  { error("Could not open the file."); }

        current_line = 1;
    }
    ch = getc(fp);
}
void return_current_char(){ ungetc(ch, fp); }

// ------------ Parser helper part  ---------------------------

PWord pWord;
std::vector<std::string> codes;
