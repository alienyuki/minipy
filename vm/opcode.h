#ifndef OPCODE_H
#define OPCODE_H

#define POP_TOP 1
#define PUSH_NULL 2
#define RETURN_VALUE 83
#define STORE_NAME 90
#define LOAD_CONST 100
#define LOAD_NAME 101
#define COMPARE_OP 107
#define JUMP_FORWARD 110
#define POP_JUMP_IF_FALSE 114
#define RETURN_CONST 121
#define BINARY_OP 122
#define LOAD_FAST 124
#define STORE_FAST 125
#define MAKE_FUNCTION 132
#define JUMP_BACKWARD 140
#define RESUME 151
#define CALL 171


#define BINOP_ADD 0
#define BINOP_FDIV 2
#define BINOP_REMINDER 6
#define BINOP_IADD 13

#endif
