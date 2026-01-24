#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

typedef struct {
    int *data;
    int top;
    size_t cap;
} STACK;

typedef struct {
    int *data;
    size_t cap;
} HEAP;

size_t get_str_len(const char *str);
char *read_file(const char *filename, long *file_size);
int store(HEAP *heap, size_t addr, int val);
int retr(HEAP heap, size_t addr, int *out_val);
int push(STACK *stack, int val);
int pop(STACK stack, int *out_val);

#endif
