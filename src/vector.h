#ifndef VECTOR_H
#define VECTOR_H

typedef struct{
    void **body;
    int len;
    int nalloc;
} Vector;

#define EMPTY_VECTOR ((Vector){})

Vector *make_vector();

Vector *make_vector_with(void *elem);

Vector *make_vector_copy(Vector *src);

void destroy_vector(Vector *vec);

void vec_push(Vector *vec, void* elem);

void *vec_pop(Vector *vec);

void vec_append(Vector *vec1, Vector *vec2);

void *vec_get(Vector *vec, int index);

void vec_set(Vector *vec, int index, void *elem);

void *vec_head(Vector *vec);

void *vec_tail(Vector *vec);

int vec_len(Vector *vec);

#endif
