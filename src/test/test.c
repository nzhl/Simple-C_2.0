#include <stdio.h>
#include <stdlib.h>


extern void test();

static void print_fail(){
    printf("Failed\n");
}

static void print_ok(){
    printf("OK\n");
}

void fail_with_msg(char *filename, int line, char *msg){
    print_fail();
    printf("%s:%d: %s\n", filename, line, msg);
    exit(0);
}

void expect_int(char *filename, int line, int expect, int actual){
    if(expect == actual){
        return;
    }
    print_fail();
    printf("%s:%d: %d expected, but got %d\n", filename, line, expect, actual);
    exit(0);
}

void expect_ptr(char *filename, int line, void *expect, void *actual){
    if(expect == actual){
        return;
    }
    print_fail();
    printf("%s:%d: %p expected, but got %p\n", filename, line, expect, actual);
    exit(0);
}

int main(){
    test();
    print_ok();
    return 0;
}
