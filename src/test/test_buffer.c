#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../buffer.h"
#include "test.h"

static void test_make_buffer(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    Buffer *buf = make_buffer();
    expecti(0, buffer_length(buf));
    expecti(0, NULL == buffer_body(buf));
    destroy_buffer(buf);
}
static void test_buffer_write(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    Buffer *buf = make_buffer();
    char *string = calloc(1001, sizeof(char));
    for(int i = 0; i < 1000; ++i){
        buffer_write(buf, 'a');
        string[i] = 'a';
    }
    expecti(0, strcmp(string, buffer_body(buf)));
    free(string);
    destroy_buffer(buf);
}

static void test_buffer_append(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    Buffer *buf = make_buffer();
    buffer_write(buf, 'a');
    buffer_write(buf, 'a');
    buffer_write(buf, 'a');
    char *string = calloc(1000, sizeof(char));
    for(int i = 0; i < 1000; ++i){
        if(i < 3){
            string[i] = 'a';
        }
        else{
            string[i] = 'b';
        }
    }
    buffer_append(buf, string + 3, 997);
    expecti(0, strcmp(string, buffer_body(buf)));
    free(string);
    destroy_buffer(buf);
}

static void test_format(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    char *str = format("This is really a big big big big big big big %d %s", 1000, "test");
    expecti(0, strcmp(str, "This is really a big big big big big big big 1000 test"));
    free(str);
}

static void test_quote_cstring(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    char *str = quote_cstring("newline \n and quote \" or backslash \\");
    //printf("%s\n", str);
    expecti(0, strcmp(str, "newline \\n and quote \\\" or backslash \\\\"));
    free(str);
}

static void test_quote_cstring_length(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    char *str = quote_cstring_length("newline \n and quote \" or backslash \\", 20);
    expecti(0, strcmp(str, "newline \\n and quote "));
    free(str);
}

static void test_quote_char(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    expecti(0, strcmp(quote_char('\\'), "\\\\"));
    expecti(0, strcmp(quote_char('\''), "\\'"));
}

void test(){
    test_make_buffer();
    test_buffer_write();
    test_buffer_append();
    test_format();
    test_quote_cstring();
    test_quote_cstring_length();
    test_quote_char();
}
