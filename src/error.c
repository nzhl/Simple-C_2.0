#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "ncc.h"
#include "file.h"


bool warning_enabled = true;
bool warning_as_error = false;

static void print_msg(char *line, char *pos, char *label, char *fmt, va_list args){
    fprintf(stderr, "[%s] ", label);
    fprintf(stderr, "%s: %s: ", line, pos);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void error_print(char *line, char *pos, char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    print_msg(line, pos, "ERROR", fmt, args);
    va_end(args);
    exit(1);
}

void warning_print(char *line, char *pos, char *fmt, ...){
    if(!warning_enabled){
        return;
    }

    char *label = warning_as_error ? "ERROR" : "WARN";
    va_list args;
    va_start(args, fmt);
    print_msg(line, pos, label, fmt, args);
    va_end(args);
    if(warning_as_error){
        exit(1);
    }
}

char *token_pos_tostring(Token *tok){
    File *file = tok->file;
    if(!file){
        return "(unknown)";
    }

    char *name = file->name ? file->name : "(unknown)";
    return format("%s:%d:%d", name, tok->line, tok->column);
}
