#ifndef DM_H
#define DM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "./da.h"
#include "./dasm/lexer.h"

#define STACK_CAPACITY 100
#define PROG_CAPACITY 100

#define HANDLE_ARITHMETIC_INSTRUCTION(type)         \
    do {                                            \
        if (dm->sp < 2) {                           \
            dm->err = Stack_UnderFlow_Error;        \
            return;                                 \
        }                                           \
        Word op2 = pop(dm);                         \
        Word op1 = pop(dm);                         \
        if (#type[0] == "/"[0] && op2 == 0) {       \
            dm->err = Division_By_Zero_Error;       \
            return;                                 \
        }                                           \
        op1 = op1 type op2;                         \
        push(dm, op1);                              \
    } while (0)


typedef int64_t Word;

typedef enum {
    INST_PUSH = 0,
    INST_DUP, 
    INST_ADD, 
    INST_SUB, 
    INST_MUL, 
    INST_DIV,
    INST_JMP,
    INST_HALT,
    INST_CMP,
    INST_JE,
    INST_JNE,
    INST_SYSCALL,
    INST_PUSH_STR,
    INST_DEC, 
    INST_INC, 
    INST_POP,
    INST_COUNT, 
} Inst_Type;

int inst_num_operands[] = {
    1,
    1,
    0,
    0,
    0,
    0,
    1,
    0,
    0,
    1,
    1,
    0,
    1,
    0,
    0,
    1,
};

typedef enum {
    NO_TYPE = 0,
    STRING_TYPE,
    WORD_TYPE,
} Operand_Type;

Operand_Type inst_op_type[] = {
    WORD_TYPE, 
    WORD_TYPE, 
    NO_TYPE, 
    NO_TYPE, 
    NO_TYPE,
    NO_TYPE,
    STRING_TYPE, 
    NO_TYPE,
    NO_TYPE, 
    STRING_TYPE, 
    STRING_TYPE, 
    NO_TYPE, 
    STRING_TYPE, 
    NO_TYPE, 
    NO_TYPE, 
    WORD_TYPE, 
};

typedef enum {
    WRITE = 0,
    READ,
} Syscall_Type;

typedef union {
    Word as_word;
    char *as_str;
} Inst_Value;

typedef struct {
    Inst_Type type;
    Inst_Value operand;
} Inst;

typedef struct {
    char *name;
    size_t address;
} Label; 

typedef enum {
    LABEL_DEFINITION, 
} Directive_Type;

typedef union  {
    Label as_label;
} Directive_Value;

typedef struct {
    Directive_Type type;
    Directive_Value value;
} Directive;

typedef enum {
    INST_TYPE, 
    DIRECTIVE_TYPE,
} Statement_Type;

typedef union {
    Inst as_inst;
    Directive as_dir;
} Statement_Value;

typedef struct { 
    Statement_Type type;
    Statement_Value value;
} Prog_Statement;

typedef enum {
    No_Error = 0, 
    Stack_OverFlow_Error, 
    Stack_UnderFlow_Error, 
    Division_By_Zero_Error,
    Label_Not_Found_Error,
    Syscall_Error,
    Invalid_Memory_Location_Error,
} Machine_Error;

typedef enum {
    WORKING = 0,
    HALTED,
} Machine_State;

typedef struct {
    Word stack[STACK_CAPACITY];
    size_t sp;

    Machine_Error err;
    Machine_State state;

    DYN_ARR Labels;
    
    Prog_Statement prog[PROG_CAPACITY];
    size_t prog_size;
    size_t ip;

    FILE *debug_file;
    FILE *output_file;
    FILE *asm_file;
} Machine;

void dm_handle_error(Machine *dm);

#endif