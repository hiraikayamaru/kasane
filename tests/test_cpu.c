#include "../emulator/cpu.h"
#include <assert.h>

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

void test_mov() {
    CPU cpu;
    cpu_init(&cpu);

    uint32_t program[] = {
        build_instruction(FMT_R, OP_MOV, 0, 1, 0, 0)
    };

    load_program(&cpu, program, 1);
    cpu.regs[1] = 0x1234;

    CPUStatus status = cpu_step(&cpu);

    assert(status == CPU_OK);
    assert(cpu.regs[0] == 0x1234);
    assert(cpu.regs[1] == 0x1234);
}

void test_add() {
    CPU cpu;
    cpu_init(&cpu);

    uint32_t program[] = {
        build_instruction(FMT_R, OP_ADD, 0, 1, 2, 0)
    };

    load_program(&cpu, program, 1);
    cpu.regs[1] = 0x1234;
    cpu.regs[2] = 0x5678;

    CPUStatus status = cpu_step(&cpu);

    assert(status == CPU_OK);
    assert(cpu.regs[0] == 0x1234 + 0x5678);
    assert(cpu.regs[1] == 0x1234);
    assert(cpu.regs[2] == 0x5678);
}

void test_mov_same_register() {
    CPU cpu;
    cpu_init(&cpu);

    uint32_t program[] = {
        build_instruction(FMT_R, OP_MOV, 0, 0, 0, 0)
    };

    load_program(&cpu, program, 1);
    cpu.regs[0] = 0x1234;

    CPUStatus status = cpu_step(&cpu);

    assert(status == CPU_OK);
    assert(cpu.regs[0] == 0x1234);
}

void test_add_same_register() {
    CPU cpu;
    cpu_init(&cpu);

    uint32_t program[] = {
        build_instruction(FMT_R, OP_ADD, 0, 0, 0, 0)
    };

    load_program(&cpu, program, 1);
    cpu.regs[0] = 0x1234;

    CPUStatus status = cpu_step(&cpu);

    assert(status == CPU_OK);
    assert(cpu.regs[0] == 0x1234 + 0x1234);
}

void test_invalid_opcode() {
    CPU cpu;
    cpu_init(&cpu);

    uint32_t program[] = {
        build_instruction(FMT_R, 31, 0, 0, 0, 0)
    };

    assert(load_program(&cpu, program, 1));

    CPUStatus status = cpu_step(&cpu);

    assert(status == CPU_INVALID_INSTRUCTION);
    assert(cpu.pc == 0);
}

int main() {
    test_mov();
    test_add();
 
    test_mov_same_register();
    test_add_same_register();

    test_invalid_opcode();
}