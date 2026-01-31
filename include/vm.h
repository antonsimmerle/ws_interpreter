#ifndef VM_H
#define VM_H

#include <stdio.h>
#include "utils.h"
#include "parser.h"

typedef struct {
    STACK stack;
    HEAP heap;
    STACK calls;
} VM_ENV;

typedef int (*op_handler)(VM_ENV*, PROG*, size_t*);

int vm(PROG *prog);

#endif
