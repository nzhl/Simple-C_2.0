
#include <stdlib.h>
#include "vector.h"


static int round_up(int n){
    // double each time until find the value larger than the result
    if(n == 0){
        return 0;
    }
    int r = 1;
    while(n > r){
        r *= 2;
    }
    return r;
}

static Vector *do_make_vector(int size){
    Vector *v = malloc(sizeof(Vector));
    size = round_up(size);
    if(size > 0){
        v->body = malloc(sizeof(void *) * size);
    }
    v->len = 0;
    v->nalloc = size;
    return v;
}

Vector *make_vector(){
    return do_make_vector(0);
}
