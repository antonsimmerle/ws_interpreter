#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "parser.h"

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

    AST ast;
    HEAP labels;
    size_t i;
    FSM_RTN ret = parser(file_content, (size_t)file_size, 
                     &i, &ast, &labels);

    free(file_content);
    return 0;
}
