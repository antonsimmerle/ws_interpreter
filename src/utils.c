#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

size_t get_str_len(const char *str) {
    size_t i = 0;
    for (; str[i] != '\0'; i++);
    return i;
}

char *read_file(const char *filename, long *file_size) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = (char *)malloc(size);
    if (buffer == NULL) {
        fclose(fp);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, size, fp);
    fclose(fp);

    if (file_size) {
        *file_size = bytes_read;
    }

    return buffer;
}

int store(HEAP *heap, size_t addr, int val) {
    if (addr >= heap->cap) {
        if (heap->cap == 0) heap->cap = 16;
        while (heap->cap <= addr) heap->cap *= 2;
        int *tmp = realloc(heap->data, heap->cap * sizeof(int));
        if (!tmp) return 1;
        heap->data = tmp;
    }
    heap->data[addr] = val;
    return 0;
}

int retr(HEAP *heap, size_t addr, int *out_val) {
    if (addr >= heap->cap) return 1;
    *out_val = heap->data[addr];
    return 0;
}

int push(STACK *stack, int val) {
    if (stack->top + 1 >= (int)stack->cap) {
        stack->cap = (stack->cap == 0) ? 16 : stack->cap * 2;

        int *tmp = realloc(stack->data, stack->cap * sizeof(int));
        if (!tmp) return 1;
        stack->data = tmp;
    }
    stack->data[++(stack->top)] = val;
    return 0;
}

int pop(STACK *stack, int *out_val) {
    if (stack->top == -1) return 1;
    *out_val = stack->data[stack->top--];
    return 0;
}
