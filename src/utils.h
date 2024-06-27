#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include "./defs.h"


const char *err_to_cstr(Error_Type err) {
    switch(err) {
        case STACK_OVERFLOW:
            return "stack overflow";
        case STACK_UNDERFLOW:
            return "stack underflow";
        case WRONG_OPERAND_TYPE:
            return "wrong operand type";
        case DIVISION_BY_ZERO:
            return "division by zero";
        case FEW_OPERANDS:
            return "too few operands";
        default:
            assert(false && "ERROR: 'err_to_cstr' failed, error type not implemented yet\n");
    }
}

const char *regs_to_cstr(Register reg) {
    switch(reg) {
        case RAX:
            return "rax";
        case RBX:
            return "rbx";
        case RCX:
            return "rcx";
        case RDX:
            return "rdx";
        default:
            assert(false && "ERROR: 'regs_to_cstr' failed, reg not implemented yet\n");
    }
}

void dm_stack(Machine *dm) {
    printf("Stack:\n");
    for (size_t i = 0; i < dm->sp; ++i) {
        printf("\t%ld\n", dm->stack[i]);
    }
}

void dm_regs(Machine *dm) {
    printf("Registers:\n");
    for (int i = 0; i  < R_COUNT; ++i) {
        printf("%s = %ld\n", regs_to_cstr((Register) i), dm->registers[i]);
    }
}




#endif