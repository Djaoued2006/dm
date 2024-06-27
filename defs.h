#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>
#include <stdio.h>
#include "./consts.h"

typedef int64_t Word;

typedef enum {
    EXIT_OKEY = 0,  
    EXIT_FAIL,
} Status_Code;

typedef enum {
    RUNNING, 
    HALTED,
    ERROR,
} State;

typedef enum {
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    WRONG_OPERAND_TYPE,
    DIVISION_BY_ZERO,
    FEW_OPERANDS,
} Error_Type;

typedef enum {
    RAX = 0,
    RBX,
    RCX,
    RDX,
    R_COUNT,
} Register;

typedef enum {
    INST_PUSH = 0,
    INST_POP,
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV,
    INST_INC,
    INST_DEC,
    INST_CMP,
    INST_JZ,
    INST_JMP,
    INST_JG,
    INST_JL,
    INST_JLE,
    INST_JGE,
    INST_MOV,
    INST_CALL,
    INST_RET,
    INST_SYSCALL,
    INST_HALT,
} Inst_Type;

typedef enum {
    NO_TYPE = 0,
    IMMEDIATE,
    LABEL, 
    REGISTER,
} Operand_Type;

typedef union {
    Word as_imm;
    char *as_label;
    Register as_reg;
} Operand_Value;

typedef struct {
    Operand_Type type;
    Operand_Value value;
} Operand;

typedef struct {
    Operand op1, op2;
} Operands;

typedef struct {
    Inst_Type type;
    Operands ops;
} Inst;

typedef struct {
    char *name;
    size_t address;
} Label;

typedef struct {
    Inst instructions[PROGRAM_CAPACITY];
    size_t prog_size;

    Label labels[LABEL_CAPACITY];
    size_t label_count;
} Program;

typedef struct {
    Word stack[STACK_CAPACITY];
    size_t sp;

    Program prog;
    
    size_t ip;

    Word registers[R_COUNT];

    State state;
    Error_Type err;

    Status_Code code;
} Machine;


#endif