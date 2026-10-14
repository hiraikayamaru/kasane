#include "cpu.c"

int main() {
    uint32_t program[] = {
        build_instruction(I_FMT, LDI_OP, 0, 0, 0, 5),
        build_instruction(I_FMT, LDI_OP, 1, 0, 0, 7),
        build_instruction(R_FMT, ADD_OP, 2, 0, 1, 0),
        build_instruction(R_FMT, MOV_OP, 3, 2, 0, 0),
        build_instruction(R_FMT, SUB_OP, 4, 3, 0, 0),
        build_instruction(I_FMT, ADDI_OP, 5, 4, 0, -1),
        build_instruction(J_FMT, HALT_OP, 0, 0, 0, 0)
    };

    size_t length = sizeof(program) /sizeof(program[0]);

    CPU cpu = {0};

    cpu_init(&cpu);

    load_program(&cpu, program, length);

    run(&cpu);

    display(&cpu);
}