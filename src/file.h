#ifndef FILE_H
#define FILE_H

typedef struct File File;

File *make_file(FILE *fp, char *name);
File *make_file_with_string(char *s);
int readc();
void unreadc(int ch);
File *current_file();
int stream_depth();
char *input_position();
void stream_stash(File *file);
void stream_unstash();

#endif
