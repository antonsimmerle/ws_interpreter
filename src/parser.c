#include "parser.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

SYM sym(unsigned char c) {
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
        [SYM_S] = { ST_START, OP_PUSH, ARG_TYPE_NUM, },
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
        [SYM_S] = { ST_START, OP_COPY, ARG_TYPE_NUM, },
        [SYM_T] = { ST_ERR, },
        [SYM_L] = { ST_START, OP_SLIDE, ARG_TYPE_NUM, },
    },
    [ST_SL] = {
        [SYM_S] = { ST_START, OP_DUPL, },
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
        [SYM_S] = { ST_START, OP_MARK, ARG_TYPE_LABEL, },
        [SYM_T] = { ST_START, OP_CALL, ARG_TYPE_LABEL, },
        [SYM_L] = { ST_START, OP_JMP, ARG_TYPE_LABEL, },
    },
    [ST_LT] = {
        [SYM_S] = { ST_START, OP_JZ, ARG_TYPE_LABEL, },
        [SYM_T] = { ST_START, OP_JN, ARG_TYPE_LABEL, },
        [SYM_L] = { ST_START, OP_RTN, },
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

int get_arg(const char *code, size_t code_size, size_t *i, ARG_TYPE arg_type, 
            int *out_arg) {
    int arg = 0;
    int sign = 1;

    int found_sign = 0;

    if (arg_type == ARG_TYPE_LABEL) arg = (arg << 1) + 1;

    for (; *i < code_size; (*i)++) {
        char c = code[*i];

        int st = (arg_type == ARG_TYPE_NUM) && !found_sign;
        switch (c) {
            case S:
                if (st) { sign = 1; found_sign = 1; }
                else arg <<= 1;
                break;
            case T:
                if (st) { sign = -1; found_sign = 1; }
                else arg = (arg << 1) + 1;
                break;
            case L:
                if (st) return 1;
                else { *out_arg = sign * arg; return 0; }
        }
    }
    return 1;
}

PRS_RTN parser(const char *code, size_t code_size, size_t *out_i, PROG *out_prog) {
    ST st = ST_START;

    size_t len = 0;
    size_t cap = 0;

    for (size_t i = 0; i < code_size; i++) {
        int j = sym((unsigned char)code[i]);
        if (j == SYM_INV) continue;

        INST cur_inst;

        TR cur_tr = tr_table[st][j];

        if (cur_tr.st_next == ST_ERR) {
            *out_i = i;
            return PRS_RTN_ERR_INST;
        }

        if (cur_tr.arg_type != ARG_TYPE_NONE) {
            i++;
            int res = get_arg(code, code_size, &i, cur_tr.arg_type, &cur_inst.arg);
            if (res) {
                *out_i = i;
                return PRS_RTN_ERR_ARG;
            }
        }
       
        if (cur_tr.st_next == ST_START) {
            cur_inst.op = cur_tr.op;

            if (len >= cap) {
                cap = (cap == 0) ? 16 : cap * 2;
                INST *tmp = realloc(out_prog->inst, cap * sizeof(INST));
                if (!tmp) {
                    free(out_prog->inst);
                    *out_i = i;
                    return PRS_RTN_ERR_ALLOC;
                }
                out_prog->inst = tmp;
            }

            out_prog->inst[len++] = cur_inst;


            printf("op: %d        arg: %d        arg_type: %d\n", cur_inst.op, cur_inst.arg, cur_tr.arg_type);

            if (cur_inst.op == OP_MARK) {
                int res = store(&out_prog->labels, cur_inst.arg, len + 1);
                if (res) return PRS_RTN_ERR_ALLOC;
            }
        }

        st = cur_tr.st_next;
    }
    out_prog->len = len;

    return st == ST_START ? PRS_RTN_OK : PRS_RTN_ERR_INST;
}
