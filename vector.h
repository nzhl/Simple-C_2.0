typedef struct {
    // vector is the array like structure (first level ptr),
    // and we need to save the ptr of that specific object (the second level ptr)
    void **body;

    int len;

    // size of alloc
    int nalloc;
} Vector;
