#include "../emulator/cpu.h"
#include <assert.h>

void display(CPU *cpu) {
    for (size_t i = 0; i < NUM_REGS; i++) {
        printf("regs[%zu]: %X\n", i, cpu->regs[i]);
    }
    printf("PC: %u\n", cpu->pc);
    if (cpu->halted) {
        puts("halted: true");
    } else {
        puts("halted: false");
    }
}

uint32_t build_instruction(uint32_t fmt, uint32_t op, uint32_t a, uint32_t b, uint32_t c, uint32_t imm) {
    uint32_t instruction = 
        (fmt << 30) | 
        (op << 25) |
        (a << 22) |
        (b << 19) |
        (c << 16) |
        (imm & 0xFFFF);
    return instruction;
}

void test_mov_same_register() {
    CPU cpu;
    cpu_init(&cpu);

    uint32_t program[] = {
        build_instruction(FMT_R, OP_MOV, 1, 1, 0, 0)
    };

    load_program(&cpu, program, 1);
    cpu.regs[1] = 0x0100;

    CPUStatus status = cpu_step(&cpu);

    assert(status == CPU_OK);
    assert(cpu.regs[1] == 0x0100);
}

int main() {
    test_mov_same_register();
}