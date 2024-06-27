#ifndef INIT_H
#define INIT_H

#include "./defs.h"

#define NO_TYPE_OPERAND                    (Operand) { .type = NO_TYPE }
#define REGISTER_OPERAND(reg)              (Operand) { .type = REGISTER, .value.as_reg = reg }
#define IMMEDIATE_OPERAND(val)             (Operand) { .type = IMMEDIATE, .value = val }
#define LABEL_OPERAND(label)               (Operand) { .type = LABEL, .value.as_label = label }


#define MAKE_INST_PUSH(OP)                 inst_init(INST_PUSH, OP, NO_TYPE_OPERAND)
#define MAKE_INST_POP(OP)                  inst_init(INST_POP, OP, NO_TYPE_OPERAND)
#define MAKE_INST_MOV(OP1, OP2)            inst_init(INST_MOV, OP1, OP2)
#define MAKE_INST_ADD(OP1, OP2)            inst_init(INST_ADD, OP1, OP2)
#define MAKE_INST_SUB(OP1, OP2)            inst_init(INST_SUB, OP1, OP2)
#define MAKE_INST_MUL(OP1, OP2)            inst_init(INST_MUL, OP1, OP2)
#define MAKE_INST_DIV(OP1, OP2)            inst_init(INST_DIV, OP1, OP2)
#define MAKE_INST_INC(OP)                  inst_init(INST_INC, OP, NO_TYPE_OPERAND)
#define MAKE_INST_DEC(OP)                  inst_init(INST_DEC, OP, NO_TYPE_OPERAND)
#define MAKE_INST_CMP(OP1, OP2)            inst_init(INST_CMP, OP1, OP2)
#define MAKE_INST_JZ(OP)                   inst_init(INST_JZ, OP, NO_TYPE_OPERAND)
#define MAKE_INST_JMP(OP)                  inst_init(INST_JMP, OP, NO_TYPE_OPERAND)
#define MAKE_INST_JGE(OP)                  inst_init(INST_JGE, OP, NO_TYPE_OPERAND)
#define MAKE_INST_JLE(OP)                  inst_init(INST_JLE, OP, NO_TYPE_OPERAND)
#define MAKE_INST_JL(OP)                   inst_init(INST_JL, OP, NO_TYPE_OPERAND)
#define MAKE_INST_JG(OP)                   inst_init(INST_JG, OP, NO_TYPE_OPERAND)
#define MAKE_INST_CALL(OP)                 inst_init(INST_CALL, OP, NO_TYPE_OPERAND)
#define MAKE_INST_RET                      inst_init(INST_RET, NO_TYPE_OPERAND, NO_TYPE_OPERAND)
#define MAKE_INST_SYSCALL                  inst_init(INST_SYSCALL, NO_TYPE_OPERAND, NO_TYPE_OPERAND)
#define MAKE_INST_HALT                     inst_init(INST_HALT, NO_TYPE_OPERAND, NO_TYPE_OPERAND)

Inst inst_init(Inst_Type type, Operand op1, Operand op2) {
    return (Inst) {
        .type = type, 
        .ops = {
            .op1 = op1,
            .op2 = op2,
        },
    };
}

#endif