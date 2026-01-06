#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

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
        if (!tmp) {
            return 1;
        }
        heap->data = tmp;
    }
    heap->data[addr] = val;
    return 0;
}
