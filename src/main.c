#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "utils.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    long file_size;
    char *file_content = read_file(argv[1], &file_size);
    if (!file_content) {
        printf("Failed to read file\n");
    }

    PROG prog;
    size_t i = 0;
    PRS_RTN ret = parser(file_content, (size_t)file_size, &i, &prog);

    switch (ret) {
        case PRS_RTN_OK:
            printf("Note: OK\n");
            break;
        case PRS_RTN_ERR_INST:
            printf("Error: Invalid instruction at %zu\n", i);
            break;
        case PRS_RTN_ERR_ARG:
            printf("Error: Invalid argument at %zu\n", i);
            break;
        case PRS_RTN_ERR_ALLOC:
            printf("Error: Memory allocation failed\n");
            break;
    }
    
    free(file_content);
    return 0;
}
