#ifndef UTILS_H
#define UTILS_H

typedef struct {
    int *data;
    size_t len;
    size_t cap;
} STACK;

typedef struct {
    int *data;
    size_t cap;
} HEAP;

char *read_file(const char *filename, long *file_size);
int store(HEAP *heap, size_t addr, int val);

#endif
