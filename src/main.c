#include <stdio.h>

#include "./dm.h"

#define ADD_INST(INST) dm_push_instruction(dm, INST)

void prog_init(Machine *dm) {
    dm->prog.labels[dm->prog.label_count++] = (Label) {
        .name = "djaoued",
        .address = 1,
    };

    ADD_INST(MAKE_INST_PUSH(IMMEDIATE_OPERAND(10)));
    ADD_INST(MAKE_INST_PUSH(IMMEDIATE_OPERAND(10)));
    ADD_INST(MAKE_INST_CMP(NO_TYPE_OPERAND, NO_TYPE_OPERAND));
    ADD_INST(MAKE_INST_JZ(LABEL_OPERAND("djaoued")));
    dm_push_instruction(dm, MAKE_INST_HALT);
}

int main(void) {
    Machine dm;
    dm_init(&dm);
    prog_init(&dm);
    dm_execute(&dm);
    dm_stack(&dm);
    dm_regs(&dm);
    return 0;
}