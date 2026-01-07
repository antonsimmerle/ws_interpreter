#ifndef PARSER_H
#define PARSER_H

#define S ' '
#define T '\t'
#define L '\n'

typedef enum {
    ST_ERR,

    ST_START,

    ST_S,
    ST_ST,
    ST_SL,

    ST_T,
    ST_TS,
    ST_TSS,
    ST_TST,
    ST_TT,
    ST_TL,
    ST_TLS,
    ST_TLT,

    ST_L,
    ST_LS,
    ST_LT,
    ST_LL,

    NUM_ST,
} ST;

typedef enum {
    SYM_S,
    SYM_T,
    SYM_L,
    SYM_INV,
} SYM;

typedef enum {
    OP_PUSH,
    OP_DUPL,
    OP_COPY,
    OP_SWAP,
    OP_DROP,
    OP_SLIDE,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    
    OP_STORE,
    OP_RETR,

    OP_OUTC,
    OP_OUTN,
    OP_INC,
    OP_INN,
    
    OP_MARK,
    OP_CALL,
    OP_JMP,
    OP_JZ,
    OP_JN,
    OP_RTN,
    OP_END,
} OP;

typedef enum {
    PRS_RTN_OK,
    PRS_RTN_ERR_INST,
    PRS_RTN_ERR_ARG,
    PRS_RTN_ERR_ALLOC,
} PRS_RTN;

typedef struct {
    OP op;
    int arg;
} INST;

typedef struct {
    ST st_next;
    OP op;
    int has_arg;
    int arg_signed;
} TR;

typedef struct {
    INST *inst;
    size_t len;
} AST; 

int get_arg(char *code, size_t code_size, int is_signed, size_t *i, int *out_arg);
PRS_RTN parser(char *code, size_t code_size,
               size_t *out_i, AST *out_ast, HEAP *out_labels);

#endif
