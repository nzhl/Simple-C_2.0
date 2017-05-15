#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "error.h"
#include "file.h"
#include "vector.h"
#include "buffer.h"

static Vector *files = &EMPTY_VECTOR;
static Vector *stashed = &EMPTY_VECTOR;

struct File{
    // File is one more abstraction
    // Either it's actually file or it's a buffer of string.
    FILE *file;
    char *string;

    char *name;
    int line;
    int column;
    int num_of_tok;
    // the last char we parse
    int last;
    int buf[3];
    int buflen;
    time_t last_modify_time;
};

File *make_file(FILE *fp, char *name){
    File *file = calloc(1, sizeof(File));
    file->file = fp;
    file->name = name;
    file->line = 1;
    file->column = 1;

    // get the status of the file
    struct stat st;
    if(fstat(fileno(fp), &st) == -1){
        error("fstat failed: %s", strerror(errno));
    }
    file->last_modify_time = st.st_mtime;
    return file;
}

File *make_file_with_string(char *s){
    File *file = calloc(1, sizeof(File));
    file->line = 1;
    file->column = 1;
    file->string = s;
    return file;
}

static void close_file(File *file){
    if(file->file){
        fclose(file->file);
    }
}

static int readc_file(File *file){
    int ch = getc(file->file);
    if(ch == EOF){
        // c11 5.1.1.2p2: source code should end in a newline
        // which means every source file should end with "\n" + EOF
        // Doing like following is make sure there is some files that
        // do not meet the requirement.
        ch = (file->last == '\n' || file->last == EOF) ? EOF : '\n';
    }
    else if(ch == '\r'){
        // c11 5.1.1.2p1: "\r\n" or "\r" are canonicalized to "\n";
        int next_ch = getc(file->file);
        if(next_ch != '\n'){
            ungetc(next_ch, file->file);
        }
        ch = '\n';
    }
    file->last = ch;
    return ch;
}

static int readc_string(File *file){
    int ch;
    if(*file->string == '\0'){
        ch = (file->last == '\n' || file->last == EOF) ? EOF : '\n';
    }
    else if(*file->string == '\r'){
        ++file->string;
        if(*file->string == '\n'){
            ++file->string;
        }
        ch = '\n';
    }else {
        ch = *file->string++;
    }
    file->last = ch;
    return ch;
}

static int get(){
    File *file = vec_tail(files);
    int ch;
    // the buffer first
    if(file->buflen > 0){
        ch = file->buf[--file->buflen];
    } else if(file->file){
        ch = readc_file(file);
    } else {
        ch = readc_string(file);
    }

    if(ch == '\n'){
        ++file->line;
        file->column = 1;
    }else if(ch != EOF){
        file->column++;
    }
    return ch;
}

int readc(){
    while(1){
        int ch = get();
        // get several files to compile
        // if finish one then just get to
        // the next
        if(ch == EOF){
            if(vec_len(files) == 1){
                return ch;
            }
            close_file(vec_pop(files));
            continue;
        }

        // c11 5.1.1.2p2 : A sequence of backslash and newline is removed
        // but it actual counts for line / column, that's why we put it here
        // rather than together with \r\n, \r, \n EOF.
        if(ch != '\\'){
            return ch;
        }
        int next_ch = get();
        if(next_ch == '\n'){
            continue;
        }
        unreadc(next_ch);
        return ch;
    }
}

void unreadc(int ch){
    if(ch == EOF){
        return;
    }

    File *file = vec_tail(files);
    assert(file->buflen < sizeof(file->buf) / sizeof(file->buf[0]));
    file->buf[file->buflen++] = ch;
    if(ch == '\n'){
        // TODO? : how can it be column = 1 ?
        // file->column = 1;
        --file->line;
    }else{
        --file->column;
    }
}

File *current_file(){
    return vec_tail(files);
}

void stream_push(File *file){
    vec_push(files, file);
}

int stream_depth(){
    return vec_len(files);
}

char *input_position(){
    if(vec_len(files) == 0){
        return "(unknown)";
    }

    File *file = vec_tail(files);
    return format("%s:%d:%d", file->name, file->line, file->column);
}

// stash current working files temporarily
// same concept from git stash
void stream_stash(File *file){
    vec_push(stashed, files);
    files = make_vector_with(file);
}

void stream_unstash(){
    files = vec_pop(stashed);
}
