#include <stdio.h>
#include <assert.h>
#include "parser.h"

int main(void) {
    const char *test_code = "  [PUSH]\t\t\t\n[-3]\n  [MARK]\t \n[2]";
    size_t code_size = get_str_len(test_code);
    size_t i;
    PROG prog;
    HEAP labels;
    PRS_RTN res = parser(test_code, code_size, &i, &prog, &labels);
    
    assert(res == PRS_RTN_OK);
    assert(prog.inst[0].op == OP_PUSH);
    assert(prog.inst[0].arg == -3);
    assert(prog.inst[1].op == OP_MARK);
    assert(prog.inst[1].arg == 2);
    assert(labels.data[2] == 3);
    
    return 0;
}
