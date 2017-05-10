#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vector.h"

#define MIN_SIZE 8

static int max(int left, int right){
    return left > right ? left : right;
}

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

static void extend_vector(Vector *vec, int delta){
    // already enough space, no need t extend
    if(vec->len + delta <= vec->nalloc){
        return;
    }

    int new_nalloc = max(round_up(vec->len), MIN_SIZE);
    void **new_body = malloc(sizeof(void *) * new_nalloc);
    memcpy(new_body, vec->body, vec->len);
    free(vec->body);
    vec->body = new_body;
    vec->nalloc = new_nalloc;
}

Vector *make_vector(){
    return do_make_vector(0);
}

Vector *make_vector_with(void *elem){
    Vector *v = do_make_vector(0);
    vec_push(v, elem);
    return v;
}

Vector *make_vector_copy(Vector *src){
    Vector *v = do_make_vector(src->len);
    memcpy(v->body, src->body, sizeof(void *) * src->len);
    v->len = src->len;
    return v;
}

void vec_push(Vector *vec, void *elem){
    extend_vector(vec, 1);
    vec->body[vec->len++] = elem;
}

void *vec_pop(Vector *vec){
    //empty case
    assert(vec->len > 0);
    return vec->body[--vec->len];
}

void vec_append(Vector *v1, Vector *v2){
    extend_vector(v1, v2->len);
    memcpy(v1->body + v1->len, v2, sizeof(void *) * v2->len);
    v1->len += v2->len;
}

void *vec_get(Vector *vec, int index){
    assert(index >= 0 && index < vec->len);
    return vec->body[index];
}

void vec_set(Vector *vec, int index, void *elem){
    assert(index >= 0 && index < vec->len);
    vec->body[index] = elem;
}

