#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#include "buffer.h"

struct Buffer{
    char *body;
    int length;
    int nalloc;
};

#define INIT_SIZE 8

Buffer *make_buffer(){
    Buffer *buf = malloc(sizeof(Buffer));
    buf->body = malloc(INIT_SIZE);
    buf->length = 0;
    buf->nalloc = INIT_SIZE;
    return buf;
}

void destroy_buffer(Buffer *buf){
    free(buf->body);
    free(buf);
}

static void extend_buffer(Buffer *buf){
    int new_nalloc = buf->nalloc * 2;
    char *new_body = malloc(new_nalloc);
    memcpy(new_body, buf->body, buf->length);
    free(buf->body);
    buf->body = new_body;
    buf->nalloc = new_nalloc;
}

char *buffer_body(Buffer *buf){
    return buf->body;
}

int buffer_length(Buffer *buf){
    return buf->length;
}

void buffer_write(Buffer *buf, char ch){
    if(buf->nalloc - 1 == buf->length){
        extend_buffer(buf);
    }
    buf->body[buf->length++] = ch;
}

void buffer_append(Buffer *buf, char *s, int length){
    for(int i = 0; i < length; ++i){
        buffer_write(buf, s[i]);
    }
}

void buffer_append_with_format(Buffer *buf, char *fmt, ...){
    va_list args;
    while(1){
        int length_left = buf->nalloc - buf->length;
        va_start(args, fmt);
        int length_needed = vsnprintf(buf->body + buf->length, length_left, fmt, args);
        va_end(args);
        if(length_needed >= length_left){
            extend_buffer(buf);
            continue;
        }
        buf->length += length_needed;
        return;
    }
}

static char *vformat(char *fmt, va_list param_list){
    Buffer *buf = make_buffer();
    va_list temp_param_list;
    while(1){
        //copy parameter pointer into temp_param_list
        va_copy(temp_param_list, param_list);

        // write at most buf->body - 1 chars with a '\0'
        // return value will be the actual needed length
        // so basic we need return value + 1 to save the '\0' as well
        int length_needed = vsnprintf(buf->body, buf->nalloc, fmt, temp_param_list);
        if(length_needed >= buf->nalloc){
            while(length_needed >= buf->nalloc){
                extend_buffer(buf);
            }
            continue;
        }
        buf->length = length_needed;
        char *str = buf->body;
        free(buf);
        return str;
    }
}

char *format(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    char *str = vformat(fmt, ap);
    va_end(ap);
    return str;
}

static char *quote(char ch){
    switch(ch){
        case '"': return "\\\"";
        case '\\': return "\\\\";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        default: return NULL;
    }
}


static void print(Buffer *buf, char ch){
    char *q = quote(ch);
    if(q){
        buffer_append_with_format(buf, "%s", q);
    }
    // ctype.h : isprint : return ch > 0x1f (judge whether its printable)
    else if(isprint(ch)){
        buffer_append_with_format(buf, "%c", ch);
    }
    else{
        // hexadecimal
        buffer_append_with_format(buf, "\\x%02x", ch);
    }
}

char *quote_cstring(char *str){
    Buffer *buf = make_buffer();
    while(*str){
        print(buf, *str++);
    }
    char *re = buf->body;
    free(buf);
    return re;
}

char *quote_cstring_length(char *str, int length){
    Buffer *buf = make_buffer();
    for(int i = 0; i < length; ++i){
        print(buf, str[i]);
    }
    char *re = buf->body;
    free(buf);
    return re;
}

char *quote_char(char ch){
    if(ch == '\\') return "\\\\";
    if(ch == '\'') return "\\'";
    return format("%c", ch);
}
