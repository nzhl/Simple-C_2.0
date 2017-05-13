#ifndef BUFFER_H
#define BUFFER_H

typedef struct Buffer Buffer;

Buffer *make_buffer();
char *buffer_body(Buffer *buf);
int buffer_length(Buffer *buf);
void buffer_write(Buffer *buf, char ch);
void buffer_append(Buffer *buf, char *s, int length);
char *format(char *fmt, ...);
char *quote_cstring(char *str);
char *quote_cstring_length(char *str, int length);
char *quote_char(char ch);

#endif
