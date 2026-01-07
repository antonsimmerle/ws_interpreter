#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "parser.h"

int get_arg(char *code, size_t code_size, int is_signed, size_t *i, int *out_arg) {
    int value = 0;
    int sign = 1;

    int seen_bit = 0;
    int found_sign = 0;

    for (; *i < code_size; (*i)++) {
        char c = code[*i];

        int st = is_signed && !found_sign;
        switch (c) {
            case S:
                if (st) { sign = 1; found_sign = 1; }
                else { value <<= 1; seen_bit = 1; }
                break;
            case T:
                if (st) { sign = -1; found_sign = 1; }
                else { value = (value << 1) + 1; seen_bit = 1; }
                break;
            case L:
                if (!seen_bit || res) return 1;
                else { *out_arg = sign * value; return 0; }
        }
    }
    return 1;
}

SYM sym(char c) {
    switch (c) {
        case S: return SYM_S;
        case T: return SYM_T;
        case L: return SYM_L;
        default: return SYM_INV;
    }
}

const TR tr_table[NUM_ST][3] = {
    [ST_START] = {
        [SYM_S] = { ST_S, },
        [SYM_T] = { ST_T, },
        [SYM_L] = { ST_L, },
    },
    [ST_S] = {
        [SYM_S] = { ST_START, OP_PUSH, .has_arg = 1, .arg_signed = 1, },
        [SYM_T] = { ST_ST, },
        [SYM_L] = { ST_SL, },
    },
    [ST_T] = {
        [SYM_S] = { ST_TS, },
        [SYM_T] = { ST_TT, },
        [SYM_L] = { ST_TL, },
    },
    [ST_L] = {
        [SYM_S] = { ST_LS, },
        [SYM_T] = { ST_LT, },
        [SYM_L] = { ST_LL, },
    },
    [ST_ST] = {
        [SYM_S] = { ST_START, OP_COPY, .has_arg = 1, .arg_signed = 1, },
        [SYM_T] = { ST_ERR, },
        [SYM_L] = { ST_START, OP_SLIDE, .has_arg = 1, .arg_signed = 1, },
    },
    [ST_SL] = {
        [SYM_S] = { ST_ERR, },
        [SYM_T] = { ST_START, OP_SWAP, },
        [SYM_L] = { ST_START, OP_DROP, },
    },
    [ST_TS] = {
        [SYM_S] = { ST_TSS, },
        [SYM_T] = { ST_TST, },
        [SYM_L] = { ST_ERR, },
    },
    [ST_TT] = {
        [SYM_S] = { ST_START, OP_STORE, },
        [SYM_T] = { ST_START, OP_RETR, },
        [SYM_L] = { ST_ERR, },
    },
    [ST_TL] = {
        [SYM_S] = { ST_TLS, },
        [SYM_T] = { ST_TLT, },
        [SYM_L] = { ST_ERR, },
    },
    [ST_LS] = {
        [SYM_S] = { ST_START, OP_MARK, .has_arg = 1, .arg_signed = 0, },
        [SYM_T] = { ST_START, OP_CALL, .has_arg = 1, .arg_signed = 0, },
        [SYM_L] = { ST_START, OP_JMP, .has_arg = 1, .arg_signed = 0, },
    },
    [ST_LT] = {
        [SYM_S] = { ST_START, OP_JZ, .has_arg = 1, .arg_signed = 0, },
        [SYM_T] = { ST_START, OP_JN, .has_arg = 1, .arg_signed = 0, },
        [SYM_L] = { ST_START, OP_RTN },
    },
    [ST_LL] = {
        [SYM_S] = { ST_ERR, },
        [SYM_T] = { ST_ERR, },
        [SYM_L] = { ST_START, OP_END, },
    },
    [ST_TSS] = {
        [SYM_S] = { ST_START, OP_ADD, },
        [SYM_T] = { ST_START, OP_SUB, },
        [SYM_L] = { ST_START, OP_MUL, },
    },
    [ST_TST] = {
        [SYM_S] = { ST_START, OP_DIV, },
        [SYM_T] = { ST_START, OP_MOD, },
        [SYM_L] = { ST_ERR, },
    },
    [ST_TLS] = {
        [SYM_S] = { ST_START, OP_OUTC, },
        [SYM_T] = { ST_START, OP_OUTN, },
        [SYM_L] = { ST_ERR, },
    },
    [ST_TLT] = {
        [SYM_S] = { ST_START, OP_INC, },
        [SYM_T] = { ST_START, OP_INN, },
        [SYM_L] = { ST_ERR, },
    },
};

PRS_RTN parser(char *code, size_t code_size,
               size_t *out_i, AST *out_ast, HEAP *out_labels) {
    ST st = ST_START;

    size_t len = 0;
    size_t cap = 0;

    for (size_t i = 0; i < code_size; i++) {
        unsigned char c = (unsigned char)code[i];
        int j = sym(c);
        if (j == SYM_INV) continue;

        INST cur_inst = {0};

        TR cur_tr = tr_table[st][j];

        if (cur_tr.st_next == ST_ERR) {
            *out_i = i;
            return PRS_RTN_ERR_INST;
        }

        if (cur_tr.has_arg) {
            i++;
            int res = get_arg(code, code_size, cur_tr.arg_signed, &i, &cur_inst.arg);
            if (res) {
                *out_i = i;
                return PRS_RTN_ERR_ARG;
            }
        }
        
        if (cur_tr.st_next == ST_START) {
            cur_inst.op = cur_tr.op;

            if (len >= cap) {
                cap = (cap == 0) ? 16 : cap * 2;
                INST *tmp = realloc(out_ast->inst, cap * sizeof(INST));
                if (!tmp) {
                    free(out_ast->inst);
                    *out_i = i;
                    return PRS_RTN_ERR_ALLOC;
                }
                out_ast->inst = tmp;
            }

            if (cur_inst.op == OP_MARK) {
                int res = store(out_labels, cur_inst.arg, len + 1);
                if (res) return PRS_RTN_ERR_ALLOC;
            }

            out_ast->inst[len++] = cur_inst;

            printf("op: %d\narg: %d\n\n", cur_inst.op, cur_inst.arg);
        }

        st = cur_tr.st_next;
    }
    out_ast->len = len;
    return st == ST_START ? PRS_RTN_OK : PRS_RTN_ERR_INST;
}
