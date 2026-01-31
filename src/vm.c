#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

int op_push(VM_ENV *env, PROG *prog, size_t *ip) {
    int arg = prog->inst[*ip].arg;
    if (push(&env->stack, arg)) return 1;
    (*ip)++;
    return 0;
}

int op_dupl(VM_ENV *env, PROG *prog, size_t *ip) {
    (void)prog;
    if (env->stack.top == -1) return 1;
    if (push(&env->stack, env->stack.data[env->stack.top])) return 1;
    (*ip)++;
    return 0;
}

int op_copy(VM_ENV *env, PROG *prog, size_t *ip) {
    int arg = prog->inst[*ip].arg;
    if (arg >= env->stack.top) return 1;
    if (push(&env->stack, env->stack.data[env->stack.top])) return 1;
    (*ip)++;
    return 0;
}

int op_swap(VM_ENV *env, PROG *prog, size_t *ip) {
    int a, b;
    if (pop(&env->stack, &a)) return 1;
    if (pop(&env->stack, &b)) return 1;
    if (push(&env->stack, a)) return 1;
    if (push(&env->stack, b)) return 1;
    (*ip)++;
    return 0;
}

int op_drop(VM_ENV *env, PROG *prog, size_t *ip) {
    if (env->stack.top == -1) return 1;
    env->stack.top--;
    (*ip)++;
    return 0;
}

int op_slide(VM_ENV *env, PROG *prog, size_t *ip) {
    int arg = prog->inst[*ip].arg;
    if (env->stack.top == -1) return 1;
    int n = arg;
    if (n > env->stack.top) n = env->stack.top;
    int top_val = env->stack.data[env->stack.top];
    env->stack.top -= n;
    env->stack.data[env->stack.top] = top_val;
    (*ip)++;
    return 0;
}

int op_add(VM_ENV *env, PROG *prog, size_t *ip) {
    int a, b;
    if (pop(&env->stack, &a)) return 1;
    if (pop(&env->stack, &b)) return 1;
    if (push(&env->stack, b + a)) return 1;
    (*ip)++;
    return 0;
}

int op_sub(VM_ENV *env, PROG *prog, size_t *ip) {
    int a, b;
    if (pop(&env->stack, &a)) return 1;
    if (pop(&env->stack, &b)) return 1;
    if (push(&env->stack, b - a)) return 1;
    (*ip)++;
    return 0;
}

int op_mul(VM_ENV *env, PROG *prog, size_t *ip) {
    int a, b;
    if (pop(&env->stack, &a)) return 1;
    if (pop(&env->stack, &b)) return 1;
    if (push(&env->stack, b * a)) return 1;
    (*ip)++;
    return 0;
}

int op_div(VM_ENV *env, PROG *prog, size_t *ip) {
    int a, b;
    if (pop(&env->stack, &a)) return 1;
    if (pop(&env->stack, &b)) return 1;
    if (push(&env->stack, b / a)) return 1;
    (*ip)++;
    return 0;
}

int op_mod(VM_ENV *env, PROG *prog, size_t *ip) {
    int a, b;
    if (pop(&env->stack, &a)) return 1;
    if (pop(&env->stack, &b)) return 1;
    if (push(&env->stack, b % a)) return 1;
    (*ip)++;
    return 0;
}

int op_store(VM_ENV *env, PROG *prog, size_t *ip) {
    int addr, val;
    if (pop(&env->stack, &addr)) return 1;
    if (pop(&env->stack, &val)) return 1;
    if (store(&env->heap, addr, val)) return 1;
    (*ip)++;
    return 0;
}

int op_retr(VM_ENV *env, PROG *prog, size_t *ip) {
    int addr, val;
    if (pop(&env->stack, &addr)) return 1;
    if (retr(&env->heap, addr, &val)) return 1;
    if (push(&env->stack, val)) return 1;
    (*ip)++;
    return 0;
}

int op_outc(VM_ENV *env, PROG *prog, size_t *ip) {
    int c;
    if (pop(&env->stack, &c)) return 1;
    printf("%c", (char)c);
    (*ip)++;
    return 0;
}

int op_outn(VM_ENV *env, PROG *prog, size_t *ip) {
    int n;
    if (pop(&env->stack, &n)) return 1;
    printf("%d", n);
    (*ip)++;
    return 0;
}

int op_inc(VM_ENV *env, PROG *prog, size_t *ip) {
    char c;
    int addr;
    if (pop(&env->stack, &addr)) return 1;
    scanf("%c", &c);
    if (store(&env->heap, addr, (int)c)) return 1;
    (*ip)++;
    return 0;
}

int op_inn(VM_ENV *env, PROG *prog, size_t *ip) {
    int n;
    int addr;
    if (pop(&env->stack, &addr)) return 1;
    scanf("%d", &n);
    if (store(&env->heap, addr, n)) return 1;
    (*ip)++;
    return 0;
}

int op_mark(VM_ENV *env, PROG *prog, size_t *ip) {
    (*ip)++;
    return 0;
}

int op_call(VM_ENV *env, PROG *prog, size_t *ip) {
    int arg = prog->inst[*ip].arg;
    if (push(&env->calls, *ip)) return 1;
    int jmp_i;
    if (retr(&prog->labels, arg, &jmp_i)) return 1;
    printf("\njmp_i: %d\n", jmp_i);
    *ip = (size_t)jmp_i;
    return 0;
}

int op_jmp(VM_ENV *env, PROG *prog, size_t *ip) {
    int arg = prog->inst[*ip].arg;
    int jmp_i;
    if (retr(&prog->labels, arg, &jmp_i)) return 1;
    *ip = (size_t)jmp_i;
    return 0;
}

int op_jz(VM_ENV *env, PROG *prog, size_t *ip) {
    int arg = prog->inst[*ip].arg;
    int top_val;
    if (pop(&env->stack, &top_val)) return 1;
    if (top_val == 0) {
        int jmp_i;
        if (retr(&prog->labels, arg, &jmp_i)) return 1;
        *ip = (size_t)jmp_i;
    } else (*ip)++;
    return 0;
}

int op_jn(VM_ENV *env, PROG *prog, size_t *ip) {
    int arg = prog->inst[*ip].arg;
    int top_val;
    if (pop(&env->stack, &top_val)) return 1;
    if (top_val < 0) {
        int jmp_i;
        if (retr(&prog->labels, arg, &jmp_i)) return 1;
        *ip = (size_t)jmp_i;
    } else (*ip)++;
    return 0;
}

int op_rtn(VM_ENV *env, PROG *prog, size_t *ip) {
    int jmp_i;
    if (pop(&env->calls, &jmp_i)) return 1;
    *ip = (size_t)jmp_i;
    return 0;
}

int op_end(VM_ENV *env, PROG *prog, size_t *ip) {
    exit(0);
}

static const op_handler handlers[NUM_OP] = {
    [OP_PUSH] = op_push,
    [OP_DUPL] = op_dupl,
    [OP_COPY] = op_copy,
    [OP_SWAP] = op_swap,
    [OP_DROP] = op_drop,
    [OP_SLIDE] = op_slide,
    [OP_ADD] = op_add,
    [OP_SUB] = op_sub,
    [OP_MUL] = op_mul,
    [OP_DIV] = op_div,
    [OP_MOD] = op_mod,
    [OP_STORE] = op_store,
    [OP_RETR] = op_retr,
    [OP_OUTC] = op_outc,
    [OP_OUTN] = op_outn,
    [OP_INC] = op_inc,
    [OP_INN] = op_inn,
    [OP_MARK] = op_mark,
    [OP_CALL] = op_call,
    [OP_JMP] = op_jmp,
    [OP_JZ] = op_jz,
    [OP_JN] = op_jn,
    [OP_RTN] = op_rtn,
    [OP_END] = op_end,
};

int vm(PROG *prog) {
    VM_ENV env = {0};
    env.stack.top = -1;

    size_t ip = 0;

    while (ip < prog->len) {
        //printf("i: %zu, op: %d, arg: %d\n", ip, prog->inst[ip].op, prog->inst[ip].arg);
        OP op = prog->inst[ip].op;
        if (handlers[op](&env, prog, &ip)) return 1;
    }

    free(env.stack.data);
    free(env.heap.data);
    free(env.calls.data);
    return 0;
}
