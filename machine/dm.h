#ifndef DM_H
#define DM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "../da.h"
#include "../utils.h"

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
    INST_JL, 
    INST_JG,
    INST_JLE, 
    INST_JGE,
    INST_COUNT, 
} Inst_Type;

static int inst_num_operands[] = {
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
    1,
    1,
    1,
    1,
    1,
    1,
    1,
};

typedef enum {
    NO_TYPE = 0,
    STRING_TYPE,
    WORD_TYPE,
    REG_TYPE,
    WORD_REG_TYPE,
} Operand_Type;

static Operand_Type inst_op_type[] = {
    WORD_REG_TYPE, 
    WORD_REG_TYPE, 
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
    REG_TYPE, 
    REG_TYPE, 
    WORD_REG_TYPE,
    STRING_TYPE, 
    STRING_TYPE, 
    STRING_TYPE,
    STRING_TYPE,
};

typedef enum {
    WRITE = 0,
    READ,
} Syscall_Type;

typedef enum {
    RAX = 0, 
    RBX,
    RCX,
    RDX,
    REGS_COUNT,
} Register;


typedef enum {
    WORD, 
    STR, 
    REG,
} Inst_Value_Type;

typedef union {
    Word as_word;
    char *as_str;
    size_t as_reg;
} Inst_Value;

typedef struct {
    Inst_Type type;
    Inst_Value operand;
    Inst_Value_Type val_type;
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

    Word regs[REGS_COUNT];

    FILE *debug_file;
    FILE *output_file;
    FILE *asm_file;
} Machine;

#define PUSH_INST(inst)                                                 \
    do {                                                                \
        Statement_Value value;                                          \
        value.as_inst = inst;                                           \
        Statement_Type type = INST_TYPE;                                \
        Prog_Statement statement = prog_statement_init(type, value);    \
        dm_push_prog_statement(dm, statement);                          \
    } while(0)

#define PUSH_DIR(label)                                                 \
    do {                                                                \
        Statement_Value value;                                          \
        value.as_dir = label;                                           \
        Statement_Type type = DIRECTIVE_TYPE;                           \
        Prog_Statement statement = prog_statement_init(type, value);    \
        dm_push_prog_statement(dm, statement);                          \
    } while(0)

static const char *inst_type_to_cstr(Inst_Type type) {
    switch(type) {
        case INST_PUSH:
            return "INST_PUSH";
        case INST_ADD:
            return "INST_ADD";
        case INST_SUB:
            return "INST_SUB";
        case INST_MUL:
            return "INST_MUL";
        case INST_DIV:
            return "INST_DIV";
        case INST_DUP:
            return "INST_DUP";
        case INST_JMP:
            return "INST_JMP";
        case INST_HALT:
            return "INST_HALT";
        case INST_CMP:
            return "INST_CMP";
        case INST_JE:
            return "INST_JE";
        case INST_JNE:
            return "INST_JNE";
        case INST_SYSCALL:  
            return "INST_SYSCALL";
        case INST_PUSH_STR:
            return "INST_PUSH_STR";
        case INST_DEC:
            return "INST_DEC";
        case INST_INC:
            return "INST_INC";
        case INST_POP:
            return "INST_POP";
        case INST_JL:
            return "INST_JL";
        case INST_JG:
            return "INST_JG";
        case INST_JLE:
            return "INST_JLE";
        case INST_JGE:
            return "INST_JGE";
        default:
            assert(false && "Error: 'inst_type_to_cstr', type not implemented yet\n");
    }
}

static const char *machine_error_to_cstr(Machine_Error err) {
    switch(err) {       
        case No_Error:
            return "No_Error";
        case Stack_OverFlow_Error:
            return "Stack_OverFlow_Error";
        case Stack_UnderFlow_Error:
            return "Stack_UnderFlow_Error";
        case Division_By_Zero_Error:
            return "Division_By_Zero_Error";
        case Label_Not_Found_Error:
            return "Label_Not_Found_Error";
        case Syscall_Error:
            return "Syscall_Error";
        case Invalid_Memory_Location_Error:
            return "Invalid_Memory_Location_Error";
        default:
            assert(false && "Error: 'machine_error_to_cstr', error not implemented yet\n");
    }
}

static Inst inst_word_init(Inst_Type type, Word operand) {
    return (Inst) {
        .type = type,
        .operand.as_word = operand,
    };
}

static Inst inst_str_init(Inst_Type type, char *name) {
    return (Inst) {
        .type = type,
        .operand.as_str = name,
    };
}

static Label label_init(char *name, size_t addr) {
    return (Label) {
        .name = name,
        .address = addr,
    };
}

static Directive dir_init(Directive_Type type, Directive_Value value) {
    return (Directive) {
        .type = type,
        .value = value,
    };
}

static Directive dir_label_init(Label label) {
    return (Directive) {
        .type = LABEL_DEFINITION,
        .value.as_label = label,
    };
}

static Prog_Statement prog_statement_init(Statement_Type type, Statement_Value value) {
    return (Prog_Statement) {
        .type = type,
        .value = value,
    };
}


#define MAKE_INST_PUSH(value)           inst_word_init(INST_PUSH, value)
#define MAKE_INST_ADD                   inst_word_init(INST_ADD, 0)
#define MAKE_INST_SUB                   inst_word_init(INST_SUB, 0)
#define MAKE_INST_MUL                   inst_word_init(INST_MUL, 0)
#define MAKE_INST_DIV                   inst_word_init(INST_DIV, 0)
#define MAKE_INST_DUP(offset)           inst_word_init(INST_DUP, offset)
#define MAKE_INST_HALT                  inst_word_init(INST_HALT, 0)
#define MAKE_INST_CMP                   inst_word_init(INST_CMP, 0)
#define MAKE_INST_SYSCALL               inst_word_init(INST_SYSCALL, 0)
#define MAKE_INST_DEC                   inst_word_init(INST_DEC, 0)
#define MAKE_INST_INC                   inst_word_init(INST_INC, 0)
#define MAKE_INST_POP(offset)           inst_word_init(INST_POP, offset)

#define MAKE_INST_JMP(name)             inst_str_init(INST_JMP, name)
#define MAKE_INST_JE(name)              inst_str_init(INST_JE, name)
#define MAKE_INST_JNE(name)             inst_str_init(INST_JNE, name)
#define MAKE_INST_JL(name)              inst_str_init(INST_JL, name)
#define MAKE_INST_JG(name)              inst_str_init(INST_JG, name)
#define MAKE_INST_JLE(name)             inst_str_init(INST_JLE, name)
#define MAKE_INST_JGE(name)             inst_str_init(INST_JGE, name)
#define MAKE_INST_PUSH_STR(str)         inst_str_init(INST_PUSH_STR, str)

#define MAKE_LABEL(name)                dir_label_init(label_init(name, dm->prog_size))

static Machine dm;

void dm_init(Machine *dm);
void dm_clean(Machine *dm);
void dm_execute(Machine *dm);
void dm_write_prog(Machine *dm);
void dm_handle_error(Machine *dm);

#endif